#include "YorzenInterface.hpp"
#include <FWorkMain.h>
#include <Yorzen/Dudas/yorzen.h>
#include <src/Overlay/Overlay.hpp>
#include <src/Globals.hpp>
#include <UIStubs.hpp>
#include <Keyauth.h>

#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

extern int keybind_menu_key;

namespace FWork {

Interface::Interface(HWND Window, HWND TargetWindow, ID3D11Device* Device, ID3D11DeviceContext* DeviceContext)
{
    Initialize(Window, TargetWindow, Device, DeviceContext);
}

Interface::~Interface()
{
    ShutDown();
}

void Interface::Initialize(HWND Window, HWND TargetWindow, ID3D11Device* Device, ID3D11DeviceContext* DeviceContext)
{
    hWindow = Window;
    hTargetWindow = TargetWindow;
    IDevice = Device;

    YorzenUI::InitializeOverlay(Window, Device, DeviceContext);

    KeyAuthClient::EnsureInit();
    Backend_LoadConfig();
    Backend_StartUtilityThread();

    bInitialized = true;
    bIsMenuOpen = true;
    YorzenMain::Login_Window = true;
    
    // Ensure overlay window receives mouse events when open by default
    if (hWindow && IsWindow(hWindow)) {
        SetWindowLongPtr(hWindow, GWL_EXSTYLE, WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_LAYERED);
        SetForegroundWindow(hWindow);
    }
}

void Interface::RenderGui()
{
    if (!bInitialized)
        return;

    SyncUIToGlobals();
    g_Globals.General.MenuOpen = bIsMenuOpen;

    if (!bIsMenuOpen) {
        Backend_RenderNotifications();
        return;
    }

    Backend_RenderNotifications();
    YorzenUI::RenderFrame();
}

void Interface::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (bIsMenuOpen && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
        return;

    if (uMsg == WM_SIZE && wParam != SIZE_MINIMIZED) {
        ResizeWidht = LOWORD(lParam);
        ResizeHeight = HIWORD(lParam);
    }
}

void Interface::HandleMenuKey()
{
    if (!hWindow || !IsWindow(hWindow))
        return;
    if (ResizeHeight != 0 || ResizeWidht != 0)
        return;

    const int menuKey = keybind_menu_key != 0 ? keybind_menu_key : YorzenKey::HideMenuKey;

    static bool menuKeyDown = false;
    if (GetAsyncKeyState(menuKey) & 0x8000) {
        if (!menuKeyDown) {
            menuKeyDown = true;
            bIsMenuOpen = !bIsMenuOpen;
            YorzenMain::Login_Window = bIsMenuOpen;

            if (bIsMenuOpen) {
                SetWindowLongPtr(hWindow, GWL_EXSTYLE, WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_LAYERED);
                SetForegroundWindow(hWindow);
                if (ImGui::GetCurrentContext()) {
                    ImGui::GetIO().MouseDrawCursor = true;
                }
            }
            else {
                SetWindowLongPtr(hWindow, GWL_EXSTYLE,
                    WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE);
                if (ImGui::GetCurrentContext()) {
                    ImGui::GetIO().MouseDrawCursor = false;
                    ImGui::GetIO().WantCaptureMouse = false;
                    ImGui::GetIO().WantCaptureKeyboard = false;
                    ImGui::ClearActiveID();
                }
                if (hTargetWindow && IsWindow(hTargetWindow))
                    SetForegroundWindow(hTargetWindow);
            }

            SetLayeredWindowAttributes(hWindow, RGB(0, 0, 0), 255, LWA_ALPHA);
            SetWindowPos(hWindow, HWND_TOPMOST, 0, 0, 0, 0,
                SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_FRAMECHANGED);
        }
    }
    else {
        menuKeyDown = false;
    }
}

void Interface::ShutDown()
{
    if (!bInitialized)
        return;

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    bInitialized = false;
}

}
