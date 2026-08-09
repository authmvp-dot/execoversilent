#pragma once
#include <Windows.h>
#include <d3d11.h>

namespace FWork
{
    class Interface
    {
    public:
        Interface(HWND Window, HWND TargetWindow, ID3D11Device* Device, ID3D11DeviceContext* DeviceContext);
        ~Interface();

        void Initialize(HWND Window, HWND TargetWindow, ID3D11Device* Device, ID3D11DeviceContext* DeviceContext);
        void RenderGui();
        void WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
        void HandleMenuKey();
        void ShutDown();
        bool GetMenuOpen() const { return bIsMenuOpen; }

    private:
        HWND hWindow = nullptr;
        HWND hTargetWindow = nullptr;
        ID3D11Device* IDevice = nullptr;
        bool bIsMenuOpen = true;
        bool bInitialized = false;

    public:
        UINT ResizeWidht = 0;
        UINT ResizeHeight = 0;
    };
}
