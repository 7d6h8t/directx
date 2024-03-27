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

  CreateGeometry();
  CreateVS();
  CreateInputLayout();
  CreatePS();
}

void Game::Update() {}

void Game::Render() {
  RenderBegin();

  // IA - VS - RS - PS - OM
  {
    // IA
    uint32_t stride = sizeof(Vertex);
    uint32_t offset = 0;
    device_context_->IASetVertexBuffers(0, 1, vertex_buffer_.GetAddressOf(),
                                        &stride, &offset);
    device_context_->IASetInputLayout(input_layout_.Get());
    device_context_->IASetPrimitiveTopology(
        D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // VS
    device_context_->VSSetShader(vertex_shader_.Get(), nullptr, 0);

    // RS
    // TODO

    // PS
    device_context_->PSSetShader(pixel_shader_.Get(), nullptr, 0);

    // OM
    // TODO

    device_context_->Draw(static_cast<uint32_t>(vertices_.size()), 0);
  }

  RenderEnd();
}

void Game::CreateGeometry() {

  // VertexData (CPU ram)
  {
    vertices_.resize(3);

    vertices_[0].position = Vec3(-0.5f, -0.5f, 0.f);
    vertices_[0].color = Color(1.f, 0.f, 0.f, 1.f);

    vertices_[1].position = Vec3(0.f, 0.5f, 0.f);
    vertices_[1].color = Color(0.f, 1.f, 0.f, 1.f);

    vertices_[2].position = Vec3(0.5f, -0.5f, 0.f);
    vertices_[2].color = Color(0.f, 0.f, 1.f, 1.f);
  }

  // VertexBuffer (cpu ram -> gpu vram)
  {
    D3D11_BUFFER_DESC desc{};
    desc.Usage = D3D11_USAGE_IMMUTABLE;
    desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    desc.ByteWidth = static_cast<uint32_t>(sizeof(Vertex) * vertices_.size());

    D3D11_SUBRESOURCE_DATA data{};
    data.pSysMem = vertices_.data();

    device_->CreateBuffer(&desc, &data, vertex_buffer_.GetAddressOf());
  }
}

void Game::CreateInputLayout() {
  D3D11_INPUT_ELEMENT_DESC layout[] = {
      {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
       D3D11_INPUT_PER_VERTEX_DATA, 0},
      {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12,
       D3D11_INPUT_PER_VERTEX_DATA, 0},
  };

  const int32_t count = ARRAYSIZE(layout);
  device_->CreateInputLayout(layout, count, vs_blob_->GetBufferPointer(),
                             vs_blob_->GetBufferSize(),
                             input_layout_.GetAddressOf());
}

void Game::CreateVS() {
  LoadShaderFromFile(L"Shaders/Default.hlsl", "VS", "vs_5_0", vs_blob_);
  HRESULT hr = device_->CreateVertexShader(vs_blob_->GetBufferPointer(),
                                           vs_blob_->GetBufferSize(), nullptr,
                                           vertex_shader_.GetAddressOf());
  CHECK(hr);
}

void Game::CreatePS() {
  LoadShaderFromFile(L"Shaders/Default.hlsl", "PS", "ps_5_0", ps_blob_);
  HRESULT hr = device_->CreatePixelShader(ps_blob_->GetBufferPointer(),
                                          ps_blob_->GetBufferSize(), nullptr,
                                          pixel_shader_.GetAddressOf());
  CHECK(hr);
}

void Game::LoadShaderFromFile(const std::wstring& path, const std::string& name,
                              const std::string& version,
                              Microsoft::WRL::ComPtr<ID3DBlob>& blob) {
  const uint32_t compile_flag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
  HRESULT hr = ::D3DCompileFromFile(
      path.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, name.c_str(),
      version.c_str(), compile_flag, 0, blob.GetAddressOf(), nullptr);

  CHECK(hr);
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
