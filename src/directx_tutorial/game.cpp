#include "pch.h"
#include "game.h"

Game::Game() {}

Game::~Game() {}

void Game::Init(HWND hwnd) {
  hwnd_ = hwnd;
  width_ = g_win_size_x;
  height_ = g_win_size_y;

  CreateDeviceAndSwapChain();
  CreateRenderTargetView();
  SetViewport();
}

void Game::Update() {}

void Game::Render() {
  RenderBegin();

  // TODO
  {
    // IA - VS - RS - PS - OM
  }

  RenderEnd();
}

void Game::CreateDeviceAndSwapChain() {
  DXGI_SWAP_CHAIN_DESC desc;
  ZeroMemory(&desc, sizeof(desc));

  desc.BufferDesc.Width = width_;
  desc.BufferDesc.Height = height_;
  desc.BufferDesc.RefreshRate.Numerator = 60;
  desc.BufferDesc.RefreshRate.Denominator = 1;
  desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
  desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
  desc.SampleDesc.Count = 1;
  desc.SampleDesc.Quality = 0;
  desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  desc.BufferCount = 1;
  desc.OutputWindow = hwnd_;
  desc.Windowed = true;
  desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

  HRESULT hr = ::D3D11CreateDeviceAndSwapChain(
      nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0,
      D3D11_SDK_VERSION, &desc, swap_chain_.GetAddressOf(),
      device_.GetAddressOf(), nullptr, device_context_.GetAddressOf());

  CHECK(hr);
}

void Game::CreateRenderTargetView() {
  HRESULT hr;

  Microsoft::WRL::ComPtr<ID3D11Texture2D> back_buffer = nullptr;
  hr = swap_chain_->GetBuffer(
      0, __uuidof(ID3D11Texture2D),
      reinterpret_cast<void**>(back_buffer.GetAddressOf()));
  CHECK(hr);

  device_->CreateRenderTargetView(back_buffer.Get(), nullptr, render_target_view_.GetAddressOf());
  CHECK(hr);
}

void Game::SetViewport() {
  viewport_.TopLeftX = 0.f;
  viewport_.TopLeftY = 0.f;
  viewport_.Width = static_cast<float>(width_);
  viewport_.Height = static_cast<float>(height_);
  viewport_.MinDepth = 0.f;
  viewport_.MaxDepth = 1.f;
}

void Game::RenderBegin() {
  device_context_->OMSetRenderTargets(1, render_target_view_.GetAddressOf(),
                                      nullptr);
  device_context_->ClearRenderTargetView(render_target_view_.Get(),
                                         clear_color);
  device_context_->RSSetViewports(1, &viewport_);
}

void Game::RenderEnd() {
  HRESULT hr = swap_chain_->Present(1, 0);
  CHECK(hr);
}
