#include "pch.h"
#include "../../include/capture/renderer.h"

#include <mfidl.h>
#include <mftransform.h>
#include <mfapi.h>

#include "../../include/com_throw.h"

using namespace capture;

void Renderer::Init(const HWND& wnd) {
  wnd_ = wnd;
  CreateDeviceAndSwapChain();
  GetBackBuffer();
}

Microsoft::WRL::ComPtr<IMFDXGIDeviceManager> Renderer::CreateDXGIDeviceManager(
    const Microsoft::WRL::ComPtr<IMFMediaSource>& source) {
  CleanUpDXGIDeviceManager();

  Microsoft::WRL::ComPtr<IMFTransform> transform = nullptr;
  com::ThrowIfFailed(source->QueryInterface(
      __uuidof(IMFTransform),
      reinterpret_cast<void**>(transform.GetAddressOf())));

  uint32_t token = 0;
  com::ThrowIfFailed(MFCreateDXGIDeviceManager(&token, &dxgi_manager_));
  com::ThrowIfFailed(dxgi_manager_->ResetDevice(device_.Get(), token));
  com::ThrowIfFailed(transform->ProcessMessage(
      MFT_MESSAGE_SET_D3D_MANAGER,
      reinterpret_cast<ULONG_PTR>(dxgi_manager_.Get())));

  return dxgi_manager_;
}

void Renderer::CleanUpDXGIDeviceManager() {
  if (dxgi_manager_) dxgi_manager_->Release();
}

void Renderer::Draw(const Microsoft::WRL::ComPtr<IMFMediaBuffer>& buffer) {
  Microsoft::WRL::ComPtr<IMFDXGIBuffer> dxgi_buffer;
  com::ThrowIfFailed(buffer->QueryInterface(
      __uuidof(IMFDXGIBuffer),
      reinterpret_cast<void**>(dxgi_buffer.GetAddressOf())));

  Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
  com::ThrowIfFailed(dxgi_buffer->GetResource(
      __uuidof(ID3D11Texture2D),
      reinterpret_cast<void**>(texture.GetAddressOf())));

  uint32_t sub_resource = 0;
  com::ThrowIfFailed(dxgi_buffer->GetSubresourceIndex(&sub_resource));

  immediate_context_->CopySubresourceRegion(
      back_buffer_.Get(), 0, 0, 0, 0, texture.Get(), sub_resource, nullptr);

  com::ThrowIfFailed(swap_chain_->Present(0, 0));
}

void Renderer::Resize(const SIZE& size) {
  size_ = size;

  immediate_context_->ClearState();
  immediate_context_->Flush();
  back_buffer_->Release();
  
  com::ThrowIfFailed(
      swap_chain_->ResizeBuffers(0, size.cx, size.cy, DXGI_FORMAT_UNKNOWN, 0));

  GetBackBuffer();
}

void Renderer::SetFps(const MFRatio& fps) { fps_ = fps; }

void Renderer::SetInputFormat(const DXGI_FORMAT& format) {
  input_format_ = format;
}

SIZE Renderer::GetSize() { return size_; }

MFRatio Renderer::GetFps() { return fps_; }

DXGI_FORMAT Renderer::GetInputFormat() { return input_format_; }

void Renderer::CreateDeviceAndSwapChain() {
  DXGI_SWAP_CHAIN_DESC desc = {};
  desc.BufferDesc.Width = size_.cx;
  desc.BufferDesc.Height = size_.cy;
  desc.BufferDesc.RefreshRate.Numerator = 60;
  desc.BufferDesc.RefreshRate.Denominator = 1;
  desc.BufferDesc.Format = output_format_;
  desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;
  desc.BufferDesc.Scaling = DXGI_MODE_SCALING_STRETCHED;
  desc.SampleDesc.Count = 1;
  desc.SampleDesc.Quality = 0;
  desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  desc.BufferCount = 1;
  desc.OutputWindow = wnd_;
  desc.Windowed = true;

  com::ThrowIfFailed(D3D11CreateDeviceAndSwapChain(
      nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0,
      D3D11_SDK_VERSION, &desc, swap_chain_.GetAddressOf(),
      device_.GetAddressOf(), nullptr, immediate_context_.GetAddressOf()));
}

void Renderer::GetBackBuffer() {
  com::ThrowIfFailed(swap_chain_->GetBuffer(
      0, __uuidof(ID3D11Texture2D),
      reinterpret_cast<void**>(back_buffer_.GetAddressOf())));
}