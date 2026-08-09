#include "Overlay.hpp"
#include <algorithm>
#include <dwmapi.h>
#include <shobjidl.h>
#include <windowsx.h>
#include <src/Globals.hpp>
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
  if (uMsg == WM_ERASEBKGND)
    return 1;

  if (uMsg == WM_NCHITTEST && !hTargetWindow) {
    POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
    ScreenToClient(hWnd, &pt);
    if (pt.y >= 0 && pt.y <= 75) {
      if (pt.x >= 110 && pt.x <= 570 && pt.y > 12) {
        return HTCLIENT;
      }
      return HTCAPTION;
    }
  }

  if (uMsg == WM_ENTERSIZEMOVE) {
    SetTimer(hWnd, 1001, 10, NULL);
  }
  else if (uMsg == WM_EXITSIZEMOVE) {
    KillTimer(hWnd, 1001);
  }
  else if (uMsg == WM_TIMER && wParam == 1001) {
    extern void RenderSingleFrame();
    RenderSingleFrame();
    return 0;
  }
  else if (uMsg == WM_PAINT) {
    extern void RenderSingleFrame();
    RenderSingleFrame();
  }

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
  } else {
    int screenW = GetSystemMetrics(SM_CXSCREEN);
    int screenH = GetSystemMetrics(SM_CYSCREEN);
    int winW = 420;
    int winH = 530;
    wTargetWindowRect.left = (screenW - winW) / 2;
    wTargetWindowRect.top = (screenH - winH) / 2;
    wTargetWindowRect.right = wTargetWindowRect.left + winW;
    wTargetWindowRect.bottom = wTargetWindowRect.top + winH;
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
    WindowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    WindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    WindowClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    WindowClass.lpszMenuName = NULL;
    WindowClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    ATOM Class = RegisterClassExA(&WindowClass);

    if (!Class) {
        DWORD errorCode = GetLastError();
        std::cout
            << "[ERROR] FrameWork::Window::Initialize::RegisterClassEx Error: "
            << std::to_string(errorCode) << std::endl;
        return;
    }

    DWORD dwStyle = hTargetWindow ? (WS_POPUP | WS_VISIBLE) : (WS_POPUP | WS_VISIBLE | WS_SYSMENU | WS_MINIMIZEBOX);
    DWORD dwExStyle = hTargetWindow ? (WS_EX_TOPMOST | WS_EX_TOOLWINDOW) : WS_EX_APPWINDOW;

    hWindow = CreateWindowExA(
        dwExStyle,
        WindowClass.lpszClassName, "Blaze Xiters",
        dwStyle, wTargetWindowRect.left,
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

    if (hTargetWindow) {
        MARGINS Margins = { wTargetWindowRect.left, wTargetWindowRect.top,
                           wTargetWindowRect.Width(), wTargetWindowRect.Height() };
        DwmExtendFrameIntoClientArea(hWindow, &Margins);
        SetLayeredWindowAttributes(hWindow, RGB(0, 0, 0), 255, LWA_ALPHA);
    } else {
        CoInitialize(NULL);
        ITaskbarList* pTaskbar = nullptr;
        if (SUCCEEDED(CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_INPROC_SERVER, IID_ITaskbarList, (void**)&pTaskbar))) {
            pTaskbar->HrInit();
            pTaskbar->AddTab(hWindow);
            pTaskbar->ActivateTab(hWindow);
            pTaskbar->Release();
        }
    }

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
  if (!hWindow)
    return;

  if (hTargetWindow) {
    WndRECT TargetWindowRect;
    GetClientRect(hTargetWindow, &TargetWindowRect);
    MapWindowPoints(hTargetWindow, nullptr,
                    reinterpret_cast<LPPOINT>(&TargetWindowRect), 2);

    g_Globals.EspConfig.Width = TargetWindowRect.Width();
    g_Globals.EspConfig.Height = TargetWindowRect.Height();

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
  } else {
    RECT currentRect;
    GetClientRect(hWindow, &currentRect);
    g_Globals.EspConfig.Width = currentRect.right - currentRect.left;
    g_Globals.EspConfig.Height = currentRect.bottom - currentRect.top;
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
  static float TransparentColor[4] = {0.f, 0.f, 0.f, 0.f};
  static float SolidColor[4] = {12.f / 255.f, 10.f / 255.f, 21.f / 255.f, 1.f};
  ID3dDeviceContext->ClearRenderTargetView(ID3dRenderTargetView,
                                           hTargetWindow ? TransparentColor : SolidColor);
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
