#include "Overlay.hpp"
#include <algorithm>
#include <dwmapi.h>
#include <example_win32_directx11/src/Globals.hpp>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <iostream>
#include <string>
#include <cstdint>

std::wstring RandomString(size_t Length) {
  auto Randchar = []() -> char {
    const char *Charset =
        "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    const size_t MaxIndex = (sizeof(Charset) - 1);
    return Charset[rand() % MaxIndex];
  };

  std::wstring Str(Length, 0);
  std::generate_n(Str.begin(), Length, Randchar);
  return Str;
}
struct WndRECT : public RECT {
  int Width() { return right - left; }
  int Height() { return bottom - top; }
};

static inline std::function<void(HWND, UINT, WPARAM, LPARAM)> pWindowProc;

bool bSettuped = false;
bool bInitialized = false;
static std::string s_overlayClassNameStorage;
bool bDeviceInitialized;
bool bRenderTargetInitialized;

HWND hWindow = nullptr;
WNDCLASSEX WindowClass;
HWND hTargetWindow = nullptr;
WndRECT wTargetWindowRect;

ID3D11Device *ID3dDevice;
ID3D11DeviceContext *ID3dDeviceContext;
IDXGISwapChain *ID3dSwapChain;
ID3D11RenderTargetView *ID3dRenderTargetView;

void CreateDeviceD3D() {

  DXGI_SWAP_CHAIN_DESC SwapChainDesc;
  ZeroMemory(&SwapChainDesc, sizeof(SwapChainDesc));
  SwapChainDesc.BufferDesc.Width = 0;
  SwapChainDesc.BufferDesc.Height = 0;
  SwapChainDesc.BufferDesc.RefreshRate.Numerator = 75;
  SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
  SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  SwapChainDesc.SampleDesc.Count = 1;
  SwapChainDesc.SampleDesc.Quality = 0;
  SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  SwapChainDesc.BufferCount = 2;
  SwapChainDesc.OutputWindow = hWindow;
  SwapChainDesc.Windowed = TRUE;
  SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
  SwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

  D3D_FEATURE_LEVEL FeatureLevel;
  const D3D_FEATURE_LEVEL FeatureLevelArray[2] = {
      D3D_FEATURE_LEVEL_11_0,
      D3D_FEATURE_LEVEL_10_0,
  };
  if (D3D11CreateDeviceAndSwapChain(
          NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, FeatureLevelArray, 2,
          D3D11_SDK_VERSION, &SwapChainDesc, &ID3dSwapChain, &ID3dDevice,
          &FeatureLevel, &ID3dDeviceContext) != S_OK) {
    std::cout
        << "[ERROR] Window::InitializeDirectX11::D3D11CreateDeviceAndSwapChain "
           "Error:" +
               GetLastError()
        << std::endl;
    return;
  }
  bDeviceInitialized = true;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam,
                            LPARAM lParam) {
  if (uMsg == WM_SIZE) {
    FWork::Overlay::UpdateWindowPos();
  }
  if (pWindowProc)
    pWindowProc(hWnd, uMsg, wParam, lParam);

  return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

namespace FWork {
void Overlay::Setup(HWND TargetHWND) {
  hTargetWindow = TargetHWND;
  if (hTargetWindow) {

    GetClientRect(hTargetWindow, &wTargetWindowRect);
    MapWindowPoints(hTargetWindow, nullptr,
                    reinterpret_cast<LPPOINT>(&wTargetWindowRect), 2);
    bSettuped = true;
  }
}

void Overlay::Initialize() {

    if (!bSettuped) {
        std::cout << "[ERROR : FrameWork::Window::Initialize] Overlay Not Settuped!"
            << std::endl;
        return;
    }

    srand(static_cast<unsigned int>(time(0)));

    const std::wstring w = RandomString(10);
    s_overlayClassNameStorage.assign(w.begin(), w.end());
    WindowClass.lpszClassName = s_overlayClassNameStorage.c_str();

    UnregisterClassA(WindowClass.lpszClassName, WindowClass.hInstance);

    WindowClass.cbSize = sizeof(WNDCLASSEXA);
    WindowClass.style = CS_HREDRAW | CS_VREDRAW;
    WindowClass.lpfnWndProc = WindowProc;
    WindowClass.cbClsExtra = 0;
    WindowClass.cbWndExtra = 0;
    WindowClass.hInstance = GetModuleHandle(NULL);
    WindowClass.hIcon = NULL;
    WindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);

    WindowClass.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));

    WindowClass.lpszMenuName = NULL;
    WindowClass.hIconSm = NULL;

    ATOM Class = RegisterClassExA(&WindowClass);

    if (!Class) {
        DWORD errorCode = GetLastError();
        std::cout
            << "[ERROR] FrameWork::Window::Initialize::RegisterClassEx Error: "
            << std::to_string(errorCode) << std::endl;
        return;
    }

    hWindow = CreateWindowExA(
        WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT |
        WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        WindowClass.lpszClassName, "",
        WS_POPUP | WS_VISIBLE, wTargetWindowRect.left,
        wTargetWindowRect.top, wTargetWindowRect.Width(),
        wTargetWindowRect.Height(), NULL, NULL,
        GetModuleHandle(NULL), NULL);

    if (!hWindow) {
        DWORD errorCode = GetLastError();
        std::cout
            << "[ERROR] FrameWork::Window::Initialize::CreateWindowEx Error:  "
            << std::to_string(errorCode) << std::endl;
        return;
    }

    MARGINS Margins = { wTargetWindowRect.left, wTargetWindowRect.top,
                       wTargetWindowRect.Width(), wTargetWindowRect.Height() };
    DwmExtendFrameIntoClientArea(hWindow, &Margins);

    SetLayeredWindowAttributes(hWindow, RGB(0, 0, 0), 255, LWA_ALPHA);

    ShowWindow(hWindow, SW_SHOWDEFAULT);
    UpdateWindow(hWindow);

    bInitialized = true;

    dxInitialize();
}

void Overlay::ShutDown() {
  dxShutDown();
  if (hWindow) {
    DestroyWindow(hWindow);
    hWindow = nullptr;
  }
  if (WindowClass.lpszClassName) {
    UnregisterClass(WindowClass.lpszClassName, WindowClass.hInstance);
    WindowClass.lpszClassName = nullptr;
  }
  bInitialized = false;
  bSettuped = false;
}

void Overlay::UpdateWindowPos() {
  if (!hTargetWindow || !hWindow)
    return;

  WndRECT TargetWindowRect;
  GetClientRect(hTargetWindow, &TargetWindowRect);
  MapWindowPoints(hTargetWindow, nullptr,
                  reinterpret_cast<LPPOINT>(&TargetWindowRect), 2);

  g_Globals.EspConfig.Width = TargetWindowRect.Width();
  g_Globals.EspConfig.Height = TargetWindowRect.Height();

  // Overlay matches emulator only â€” keybind HUD uses a separate desktop window.
  RECT currentRect;
  GetWindowRect(hWindow, &currentRect);
  int currentWidth = currentRect.right - currentRect.left;
  int currentHeight = currentRect.bottom - currentRect.top;

  if (currentRect.left != TargetWindowRect.left ||
      currentRect.top != TargetWindowRect.top ||
      currentWidth != TargetWindowRect.Width() ||
      currentHeight != TargetWindowRect.Height()) {
    MoveWindow(hWindow, TargetWindowRect.left, TargetWindowRect.top,
               TargetWindowRect.Width(), TargetWindowRect.Height(), FALSE);
  }
}

void Overlay::SetupWindowProcHook(
    std::function<void(HWND, UINT, WPARAM, LPARAM)> Funtion) {
  pWindowProc = Funtion;
}

void Overlay::dxInitialize() {
  CreateDeviceD3D();
  if (bDeviceInitialized) {
    dxCreateRenderTarget();
  }
}

void Overlay::dxRefresh() {
  if (!ID3dDeviceContext || !ID3dRenderTargetView)
    return;

  ID3dDeviceContext->OMSetRenderTargets(1, &ID3dRenderTargetView, nullptr);
  static float TransparentColor[4] = {0, 0, 0, 0};
  ID3dDeviceContext->ClearRenderTargetView(ID3dRenderTargetView,
                                           TransparentColor);
}

void Overlay::dxPresent() {
  if (!ID3dSwapChain)
    return;

  ID3dSwapChain->Present(0, 0);
}

void Overlay::dxShutDown() {
  dxCleanupRenderTarget();
  dxCleanupDeviceD3D();
}

void Overlay::dxCreateRenderTarget() {
  if (!ID3dSwapChain || !ID3dDevice)
    return;

  ID3D11Texture2D *pBackBuffer;
  if (FAILED(ID3dSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer))))
    return;
  ID3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &ID3dRenderTargetView);
  pBackBuffer->Release();
  bRenderTargetInitialized = true;
  if (ID3dDeviceContext) {
    ID3dDeviceContext->OMSetRenderTargets(1, &ID3dRenderTargetView, nullptr);
  }
}

void Overlay::dxCleanupRenderTarget() {

  if (ID3dRenderTargetView) {
    ID3dRenderTargetView->Release();
    ID3dRenderTargetView = NULL;
  }

  bRenderTargetInitialized = false;
}

void Overlay::dxCleanupDeviceD3D() {

  if (ID3dRenderTargetView) {
    ID3dRenderTargetView->Release();
    ID3dRenderTargetView = NULL;
  }
  if (ID3dSwapChain) {
    ID3dSwapChain->Release();
    ID3dSwapChain = NULL;
  }
  if (ID3dDeviceContext) {
    ID3dDeviceContext->Release();
    ID3dDeviceContext = NULL;
  }
  if (ID3dDevice) {
    ID3dDevice->Release();
    ID3dDevice = NULL;
  }

  ImGui_ImplDX11_Shutdown();
  ImGui_ImplWin32_Shutdown();
  ImGui::DestroyContext();
  bDeviceInitialized = false;
}

bool Overlay::IsSettuped() { return bSettuped; }
bool Overlay::IsInitialized() {
  return bInitialized && bDeviceInitialized && bRenderTargetInitialized;
}
HWND Overlay::GetOverlayWindow() { return hWindow; }
HWND Overlay::GetTargetWindow() { return hTargetWindow; }

ID3D11Device *Overlay::dxGetDevice() { return ID3dDevice; }
ID3D11DeviceContext *Overlay::dxGetDeviceContext() { return ID3dDeviceContext; }
IDXGISwapChain *Overlay::dxGetSwapChain() { return ID3dSwapChain; }
ID3D11RenderTargetView *Overlay::dxGetRenderTarget() {
  return ID3dRenderTargetView;
}
} // namespace FWork
