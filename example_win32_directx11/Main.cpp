#define IMGUI_DEFINE_MATH_OPERATORS

#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include <D3DX11tex.h>
#include <d3d11.h>
#include <windows.h>
#pragma comment(lib, "D3DX11.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")
#include <thread>
#include <atomic>
#include <string>
#include <iostream>

#include "ext/MinHook/include/MinHook.h"
#include <TlHelp32.h>
#include <Psapi.h>
#pragma comment(lib, "Psapi.lib")
#include <VersionHelpers.h>
#include <winver.h>
#pragma comment(lib, "Version.lib")
#include <wininet.h>
#pragma comment(lib, "wininet.lib")
#include <sstream>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <sddl.h>
#pragma comment(lib, "advapi32.lib")

#include <src/adb/adb.hpp>
#include <src/Overlay/Overlay.hpp>
#include <src/ui/YorzenInterface.hpp>
#include <src/Backend/SyncGlobals.hpp>
#include <src/Globals.hpp>
#include <src/Globals.hpp>
#include "src/Overlay/Render.hpp"
#include <src/Fonts/Fonts.hpp>
#include <imgui_settings.h>

using namespace adb;

HMODULE g_hModule = nullptr;
bool bShouldUnload = false;
FWork::Interface* g_pInterface = nullptr;

std::atomic<bool> g_AdbReady{ false };
std::atomic<bool> g_AdbFailed{ false };
std::atomic<bool> g_AuthStarted{ false };
std::atomic<bool> g_AuthDone{ false };
std::atomic<bool> g_AuthOK{ false };

static void ApplyEmulatorPerformanceMode()
{
    static bool s_lastPerf = false;
    const bool perf = g_Globals.General.DisableAllEffects;
    if (perf == s_lastPerf)
        return;
    s_lastPerf = perf;
    SetPriorityClass(GetCurrentProcess(), perf ? BELOW_NORMAL_PRIORITY_CLASS : NORMAL_PRIORITY_CLASS);
}

DWORD WINAPI Unload()
{
    adb::KillEmulatorAndAdbOnExit();

    if (g_pInterface) {
        g_pInterface->ShutDown();
        g_pInterface = nullptr;
    }

    if (MemoryUtils::ogPhysRead) {
        MH_DisableHook((LPVOID)MemoryUtils::ogPhysRead);
        MH_RemoveHook((LPVOID)MemoryUtils::ogPhysRead);
    }

    MH_Uninitialize();
    bShouldUnload = true;

    if (g_hModule)
        FreeLibraryAndExitThread(g_hModule, 0);

    return 0;
}

bool MemoryInit = false;

void Memory()
{
    // Memory initialization removed for UI-only version
    MemoryInit = true;
}

void adbInit()
{
    // ADB init removed for UI-only version
    g_AdbReady = true;
}

void authInit()
{
    if (g_AuthStarted.exchange(true))
        return;

    while (!g_AuthDone.load())
        Sleep(50);

    g_AuthOK = g_AuthOK.load();
    g_AuthDone = true;
}

namespace Cheat {

void Initialize()
{
    Memory();
    if (!MemoryInit)
        MessageBoxW(NULL, L"Error Initialize Memory", L"Error", NULL);

    std::thread([] { authInit(); }).detach();

    FWork::Overlay::Setup(Render::FindRenderWindow());
    FWork::Overlay::Initialize();

    if (!FWork::Overlay::IsInitialized())
        return;

    if (!FWork::Overlay::dxGetDevice() || !FWork::Overlay::GetOverlayWindow())
        return;

    FWork::Interface Interface(
        FWork::Overlay::GetOverlayWindow(),
        FWork::Overlay::GetTargetWindow(),
        FWork::Overlay::dxGetDevice(),
        FWork::Overlay::dxGetDeviceContext());

    g_pInterface = &Interface;

    FWork::Overlay::SetupWindowProcHook(std::bind(
        &FWork::Interface::WindowProc, &Interface,
        std::placeholders::_1, std::placeholders::_2,
        std::placeholders::_3, std::placeholders::_4));

    MSG Message{};
    while (Message.message != WM_QUIT)
    {
        HWND hWindow = FWork::Overlay::GetOverlayWindow();
        if (hWindow == nullptr)
            break;

        if (PeekMessage(&Message, hWindow, NULL, NULL, PM_REMOVE)) {
            TranslateMessage(&Message);
            DispatchMessage(&Message);
        }

        if (ImGui::GetCurrentContext())
            ImGui::GetIO().MouseDrawCursor = Interface.GetMenuOpen();

        if (Interface.ResizeHeight != 0 || Interface.ResizeWidht != 0) {
            FWork::Overlay::dxCleanupRenderTarget();
            if (IDXGISwapChain* pSwapChain = FWork::Overlay::dxGetSwapChain()) {
                pSwapChain->ResizeBuffers(0, Interface.ResizeWidht, Interface.ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
                Interface.ResizeHeight = Interface.ResizeWidht = 0;
                FWork::Overlay::dxCreateRenderTarget();
            }
        }

        Interface.HandleMenuKey();
        FWork::Overlay::UpdateWindowPos();
        ApplyEmulatorPerformanceMode();

        if (g_Globals.EspConfig.Width <= 0 || g_Globals.EspConfig.Height <= 0 ||
            IsIconic(FWork::Overlay::GetTargetWindow())) {
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
            continue;
        }

        static bool CaptureBypassOn = false;
        if (g_Globals.General.Capture != CaptureBypassOn) {
            CaptureBypassOn = g_Globals.General.Capture;
            SetWindowDisplayAffinity(FWork::Overlay::GetOverlayWindow(),
                CaptureBypassOn ? WDA_EXCLUDEFROMCAPTURE : WDA_NONE);
        }

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        {
            ImGuiIO& ioFrame = ImGui::GetIO();
            ioFrame.DeltaTime = ImMin(ioFrame.DeltaTime, 1.0f / 36.0f);
        }

        SyncUIToGlobals();

        Interface.RenderGui();

        if (g_Globals.Misc.ShowAimbotFov && g_Globals.AimBot.Fov > 0.f &&
            g_Globals.EspConfig.Width > 0 && g_Globals.EspConfig.Height > 0) {
            const ImColor fovColor(
                g_Globals.Misc.AimbotFovColor[0],
                g_Globals.Misc.AimbotFovColor[1],
                g_Globals.Misc.AimbotFovColor[2],
                g_Globals.Misc.AimbotFovColor[3]);

            ImDrawList* fovDraw = ImGui::GetForegroundDrawList();
            fovDraw->AddCircle(
                ImVec2(g_Globals.EspConfig.Width * 0.5f, g_Globals.EspConfig.Height * 0.5f),
                g_Globals.AimBot.Fov, fovColor, 64, 1.5f);
        }

        ImGui::EndFrame();
        ImGui::Render();
        FWork::Overlay::dxRefresh();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        if (IDXGISwapChain* pSwapChain = FWork::Overlay::dxGetSwapChain())
            pSwapChain->Present(1, 0);

        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        if (g_Globals.General.ShutDown) {
            Unload();
            return;
        }
    }
}

}

int APIENTRY wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int)
{
#ifdef _DEBUG
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
#endif

    Cheat::Initialize();

    while (!bShouldUnload && !g_Globals.General.ShutDown)
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

    return 0;
}
