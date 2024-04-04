#include "pch.h"
#include "../../include/capture/controller.h"

#include <Shlwapi.h>
#include <mfapi.h>
#include <Mferror.h>

#include "../../include/com_throw.h"

using namespace capture;

void Controller::Init(const HWND& wnd) { renderer_.Init(wnd); }

void Controller::Open(const Microsoft::WRL::ComPtr<IMFActivate>& activate) {
  Close();

  std::lock_guard<std::mutex> lock(mutex_);

  Microsoft::WRL::ComPtr<IMFMediaSource> source = nullptr;
  Microsoft::WRL::ComPtr<IMFAttributes> attributes = nullptr;

  com::ThrowIfFailed(activate->ActivateObject(
      __uuidof(IMFMediaSource),
      reinterpret_cast<void**>(source.GetAddressOf())));

  auto dxgi_manager = renderer_.CreateDXGIDeviceManager(source);

  com::ThrowIfFailed(MFCreateAttributes(&attributes, 5));
  com::ThrowIfFailed(attributes->SetUnknown(MF_SOURCE_READER_ASYNC_CALLBACK, this));
  com::ThrowIfFailed(attributes->SetUINT32(
      MF_SOURCE_READER_ENABLE_ADVANCED_VIDEO_PROCESSING, true));
  com::ThrowIfFailed(attributes->SetUINT32(
      MF_SOURCE_READER_DISCONNECT_MEDIASOURCE_ON_SHUTDOWN, true));
  com::ThrowIfFailed(attributes->SetUINT32(MF_READWRITE_DISABLE_CONVERTERS, false));
  com::ThrowIfFailed(attributes->SetUnknown(MF_SOURCE_READER_D3D_MANAGER, dxgi_manager.Get()));

  com::ThrowIfFailed(MFCreateSourceReaderFromMediaSource(
      source.Get(), attributes.Get(), reader_.GetAddressOf()));

  com::ThrowIfFailed(
      reader_->SetStreamSelection(MF_SOURCE_READER_FIRST_VIDEO_STREAM, true));
}

void Controller::Close() {
  std::lock_guard<std::mutex> lock(mutex_);
  if (reader_) reader_->Release();
}

void Controller::SetMediaType(const uint32_t type_index) {
  if (!reader_) throw std::exception("[error] SourceReader is nullptr");

  Microsoft::WRL::ComPtr<IMFMediaType> type = nullptr;
  com::ThrowIfFailed(reader_->GetNativeMediaType(
      MF_SOURCE_READER_FIRST_VIDEO_STREAM, type_index, &type));

  MFRatio fps{0};
  com::ThrowIfFailed(MFGetAttributeRatio(
      type.Get(), MF_MT_FRAME_RATE, reinterpret_cast<uint32_t*>(&fps.Numerator),
      reinterpret_cast<uint32_t*>(&fps.Denominator)));

  uint32_t width = 0, height = 0;
  com::ThrowIfFailed(MFGetAttributeSize(type.Get(), MF_MT_FRAME_SIZE, &width, &height));

  GUID sub_type{};
  com::ThrowIfFailed(type->GetGUID(MF_MT_SUBTYPE, &sub_type));

  DXGI_FORMAT format = MFMapDX9FormatToDXGIFormat(sub_type.Data1);
  renderer_.SetInputFormat(format);

  Microsoft::WRL::ComPtr<IMFMediaType> output_type;
  com::ThrowIfFailed(MFCreateMediaType(&output_type));
  com::ThrowIfFailed(output_type->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video));
  com::ThrowIfFailed(output_type->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_ARGB32));

  com::ThrowIfFailed(output_type->SetUINT32(MF_MT_INTERLACE_MODE,
                                            MFVideoInterlace_Progressive));
  com::ThrowIfFailed(output_type->SetUINT64(MF_MT_FRAME_SIZE, PackSize(width, height)));
  com::ThrowIfFailed(output_type->SetUINT64(
      MF_MT_FRAME_RATE, PackSize(fps.Numerator, fps.Denominator)));

  com::ThrowIfFailed(reader_->SetCurrentMediaType(
      MF_SOURCE_READER_FIRST_VIDEO_STREAM, nullptr, output_type.Get()));

  renderer_.Resize({static_cast<long>(width), static_cast<long>(height)});
  renderer_.SetFps(fps);
}

std::deque<Microsoft::WRL::ComPtr<IMFMediaType>>
Controller::EnumerateMediaTypes() {
  if (!reader_) return {};

  std::deque<Microsoft::WRL::ComPtr<IMFMediaType>> types;
  HRESULT hr = S_OK;
  for (uint32_t media_type_index = 0; SUCCEEDED(hr); ++media_type_index) {
    Microsoft::WRL::ComPtr<IMFMediaType> type = nullptr;

    if (SUCCEEDED(hr))
      hr = reader_->GetNativeMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM,
                                       media_type_index, type.GetAddressOf());
    if (hr == MF_E_NO_MORE_TYPES) {
      hr = S_OK;
      break;
    } else if (SUCCEEDED(hr)) {
      GUID major_type{};
      hr = type->GetGUID(MF_MT_MAJOR_TYPE, &major_type);
      if (major_type != MFMediaType_Video) continue;

      types.push_back(type);
    }
  }

  return types;
}

void Controller::ReadFrame() {
  com::ThrowIfFailed(reader_->ReadSample(MF_SOURCE_READER_FIRST_VIDEO_STREAM, 0,
                                         nullptr, nullptr, nullptr, nullptr));  
}

HRESULT STDMETHODCALLTYPE Controller::QueryInterface(REFIID iid, void** ppv) {
  static const QITAB qit[] = {
      QITABENT(Controller, IMFSourceReaderCallback),
      {0},
  };
  return QISearch(this, qit, iid, ppv);
}

ULONG STDMETHODCALLTYPE Controller::AddRef() {
  return InterlockedIncrement(&ref_count_);
}

ULONG STDMETHODCALLTYPE Controller::Release() {
  ULONG count = InterlockedDecrement(&ref_count_);
  if (count == 0) delete this;
  return count;
}

HRESULT STDMETHODCALLTYPE Controller::OnEvent(DWORD, IMFMediaEvent*) {
  return S_OK;
}

HRESULT STDMETHODCALLTYPE Controller::OnFlush(DWORD) { return S_OK; }

HRESULT STDMETHODCALLTYPE Controller::OnReadSample(HRESULT hrStatus,
                                                DWORD dwStreamIndex,
                                                DWORD dwStreamFlags,
                                                LONGLONG llTimestamp,
                                                IMFSample* pSample) {
  std::lock_guard<std::mutex> lock(mutex_);

  HRESULT hr = S_OK;
  Microsoft::WRL::ComPtr<IMFMediaBuffer> buffer = nullptr;

  if (FAILED(hrStatus)) hr = hrStatus;

  if (SUCCEEDED(hr)) {
    if (pSample) {
      com::ThrowIfFailed(pSample->ConvertToContiguousBuffer(buffer.GetAddressOf()));
      renderer_.Draw(buffer);
    }
  }

  if (SUCCEEDED(hr)) ReadFrame();
  return hr;
}