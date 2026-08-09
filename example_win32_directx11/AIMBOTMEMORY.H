#pragma once
#include <windows.h>

class AimbotMemory {
public:
    void SetEmulatorTargetWindow(HWND) {}
    BOOL AttackProcess(BOOL) { return TRUE; }
    BOOL GetEmulatorRunning() { return TRUE; }
    void ActivateWallhack() {}
    void OFFWallhack() {}
    void ActivateFastlanding() {}
    void OFFFastlanding() {}
    void ActivateCamera() {}
    void OFFCamera() {}
    void SniperSwitchon() {}
    void SniperSwitchoff() {}
};

class AimMemory {
public:
};
