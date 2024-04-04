#pragma once
#include <mfidl.h>
#include <mfreadwrite.h>
#include <wrl/client.h>
#include <deque>
#include <string>
#include <mutex>

#include "../export.h"
#include "renderer.h"

#pragma warning(push)
#pragma warning(disable : 4251)

struct IMFSample;
struct IMFMediaEvent;
struct IMFMediaType;

namespace capture {
class CAPTURE_API Controller final : public IMFSourceReaderCallback {
 public:
  void Init(const HWND& wnd);
  void Open(const Microsoft::WRL::ComPtr<IMFActivate>& activate);
  void Close();

  void SetMediaType(const uint32_t type_index);
  std::deque<Microsoft::WRL::ComPtr<IMFMediaType>> EnumerateMediaTypes();
  void ReadFrame();

  HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** ppv) override;
  ULONG STDMETHODCALLTYPE AddRef() override;
  ULONG STDMETHODCALLTYPE Release() override;
  HRESULT STDMETHODCALLTYPE OnEvent(DWORD, IMFMediaEvent*) override;
  HRESULT STDMETHODCALLTYPE OnFlush(DWORD) override;

  // callback read buffer
  HRESULT STDMETHODCALLTYPE OnReadSample(HRESULT hrStatus, DWORD dwStreamIndex,
                                         DWORD dwStreamFlags,
                                         LONGLONG llTimestamp,
                                         IMFSample* pSample) override;

 private:
  Microsoft::WRL::ComPtr<IMFSourceReader> reader_ = nullptr;
  Renderer renderer_{};

  std::mutex mutex_{};
  uint32_t ref_count_ = 1;
};
}  // namespace display

#pragma warning(pop)