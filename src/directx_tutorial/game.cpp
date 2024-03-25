#include "pch.h"
#include "game.h"

Game::Game() {}

Game::~Game() {}

void Game::Init(HWND hwnd) {
  hwnd_ = hwnd;
  width_ = g_win_size_x;
  height_ = g_win_size_y;
}

void Game::Update() {}

void Game::Render() {}
