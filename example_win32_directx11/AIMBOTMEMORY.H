#pragma once
#include <windows.h>

class AimbotMemory {
public:
    void SetEmulatorTargetWindow(HWND) {}
    BOOL AttackProcess(BOOL) { return TRUE; }
    BOOL GetEmulatorRunning() { return TRUE; }

    // Wallhack
    void ActivateWallhack() {}
    void OFFWallhack() {}
    void SaveWallhackAoB() {}

    // Fast Landing
    void ActivateFastlanding() {}
    void OFFFastlanding() {}
    void SaveFastlandingAoB() {}

    // Camera
    void ActivateCamera() {}
    void OFFCamera() {}
    void SaveCameraAoB() {}

    // Speed
    void ActivateSpeed() {}
    void OFFSpeed() {}

    // Sniper Switch
    void SniperSwitchon() {}
    void SniperSwitchoff() {}

    // Glitch Fire
    void GlitchFireON() {}
    void GlitchFireOFF() {}

    // Guest Reset
    void GuestResetON() {}
    void GuestResetOFF() {}
};

class AimMemory {
public:
};
