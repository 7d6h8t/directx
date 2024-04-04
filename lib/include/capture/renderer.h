#pragma once
#include <windef.h>
#include <wrl/client.h>
#include <mfobjects.h>
#include <dxgiformat.h>
#include <d3d11.h>

struct IMFMediaSource;
struct IMFMediaBuffer;
struct IMFDXGIDeviceManager;
struct IDXGISwapChain;

namespace capture {
class Renderer final {
 public:
  void Init(const HWND& wnd);
  Microsoft::WRL::ComPtr<IMFDXGIDeviceManager> CreateDXGIDeviceManager(
      const Microsoft::WRL::ComPtr<IMFMediaSource>& source);
  void CleanUpDXGIDeviceManager();

  void Draw(const Microsoft::WRL::ComPtr<IMFMediaBuffer>& buffer);
  void Resize(const SIZE& size);

  void SetFps(const MFRatio& fps);
  void SetInputFormat(const DXGI_FORMAT& format);

  SIZE GetSize();
  MFRatio GetFps();
  DXGI_FORMAT GetInputFormat();

 private:
  void CreateDeviceAndSwapChain();
  void GetBackBuffer();

  HWND wnd_ = nullptr;
  SIZE size_{};
  MFRatio fps_{};
  DXGI_FORMAT input_format_ = DXGI_FORMAT_UNKNOWN;
  DXGI_FORMAT output_format_ = DXGI_FORMAT_B8G8R8A8_UNORM;

  Microsoft::WRL::ComPtr<IDXGISwapChain> swap_chain_ = nullptr;
  Microsoft::WRL::ComPtr<ID3D11Device> device_ = nullptr;
  Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediate_context_ = nullptr;
  Microsoft::WRL::ComPtr<ID3D11Texture2D> back_buffer_ = nullptr;
  Microsoft::WRL::ComPtr<IMFDXGIDeviceManager> dxgi_manager_ = nullptr;
};
}  // namespace display