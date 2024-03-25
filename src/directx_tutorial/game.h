#pragma once
class Game {
 public:
  Game();
  ~Game();

  void Init(HWND hwnd);
  void Update();
  void Render();
};
