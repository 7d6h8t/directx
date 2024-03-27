#pragma once
class Game {
 public:
  Game();
  ~Game();

  void Init(HWND hwnd);
  void Update();
  void Render();

 private:
  void CreateGeometry();
  void CreateInputLayout();

  void CreateVS();
  void CreatePS();

  void LoadShaderFromFile(const std::wstring& path, const std::string& name,
                          const std::string& version,
                          Microsoft::WRL::ComPtr<ID3DBlob>& blob);

  void CreateDeviceAndSwapChain();
  void CreateRenderTargetView();
  void SetViewport();
  void RenderBegin();
  void RenderEnd();

  HWND hwnd_ = nullptr;
  uint32_t width_ = 0;
  uint32_t height_ = 0;

  // Device & SwapChain
  Microsoft::WRL::ComPtr<ID3D11Device> device_ = nullptr;
  Microsoft::WRL::ComPtr<ID3D11DeviceContext> device_context_ = nullptr;
  Microsoft::WRL::ComPtr<IDXGISwapChain> swap_chain_ = nullptr;

  // RTV
  Microsoft::WRL::ComPtr<ID3D11RenderTargetView> render_target_view_ = nullptr;

  // Misc
  D3D11_VIEWPORT viewport_ = {0};
  float clear_color[4] = {0.5f, 0.5f, 0.5f, 0.5f};

  // Geometry
  std::vector<Vertex> vertices_;
  Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer_ = nullptr;
  Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout_ = nullptr;

  // VS
  Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader_ = nullptr;
  Microsoft::WRL::ComPtr<ID3DBlob> vs_blob_ = nullptr;

  // PS
  Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader_ = nullptr;
  Microsoft::WRL::ComPtr<ID3DBlob> ps_blob_ = nullptr;

  // [CPU <-> RAM] [GPU <-> VRAM]
};
