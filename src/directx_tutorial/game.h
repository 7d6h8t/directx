#pragma once
class Game {
 public:
  Game();
  ~Game();

  void Init(HWND hwnd);
  void Update();
  void Render();

 private:
  HWND hwnd_;
  uint32_t width_ = 0;
  uint32_t height_ = 0;

  // dx
};
