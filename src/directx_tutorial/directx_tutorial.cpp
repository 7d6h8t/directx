#include "pch.h"
#include "framework.h"
#include "directx_tutorial.h"
#include "game.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;  // current instance
HWND h_wnd;

ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                      _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine,
                      _In_ int nCmdShow) {
  MyRegisterClass(hInstance);

  if (!InitInstance(hInstance, nCmdShow)) {
    return FALSE;
  }

  Game game;
  game.Init(h_wnd);

  MSG msg{};

  // Main message loop:
  while (msg.message != WM_QUIT) {
    if (::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    } else {
      game.Update();
      game.Render();
    }
  }

  return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance) {
  WNDCLASSEXW wcex;

  wcex.cbSize = sizeof(WNDCLASSEX);

  wcex.style = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc = WndProc;
  wcex.cbClsExtra = 0;
  wcex.cbWndExtra = 0;
  wcex.hInstance = hInstance;
  wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DIRECTXTUTORIAL));
  wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
  wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
  wcex.lpszMenuName = nullptr;
  wcex.lpszClassName = L"DirectX tutorial";
  wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

  return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
  hInst = hInstance;

  RECT window_rect = {0, 0, g_win_size_x, g_win_size_y};
  ::AdjustWindowRect(&window_rect, WS_OVERLAPPEDWINDOW, false);

  h_wnd = CreateWindowW(L"DirectX tutorial", L"Client", WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT, 0, window_rect.right - window_rect.left,
                        window_rect.bottom - window_rect.top, nullptr, nullptr,
                        hInstance, nullptr);

  if (!h_wnd) {
    return FALSE;
  }

  ::ShowWindow(h_wnd, nCmdShow);
  ::UpdateWindow(h_wnd);

  return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam,
                         LPARAM lParam) {
  switch (message) {
    case WM_COMMAND: {
      int wmId = LOWORD(wParam);
      switch (wmId) {
        case IDM_EXIT:
          DestroyWindow(hWnd);
          break;
        default:
          return DefWindowProc(hWnd, message, wParam, lParam);
      }
    } break;
    case WM_PAINT: {
      PAINTSTRUCT ps;
      HDC hdc = BeginPaint(hWnd, &ps);
      EndPaint(hWnd, &ps);
    } break;
    case WM_DESTROY:
      PostQuitMessage(0);
      break;
    default:
      return DefWindowProc(hWnd, message, wParam, lParam);
  }
  return 0;
}