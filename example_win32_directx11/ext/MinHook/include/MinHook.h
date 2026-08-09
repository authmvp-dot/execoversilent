#pragma once

// MinHook stub - inline no-op for clean compilation
#include <windows.h>

typedef int MH_STATUS;
#define MH_OK 0
#define MH_ERROR_ALREADY_INITIALIZED 1

inline MH_STATUS MH_Initialize() { return MH_OK; }
inline MH_STATUS MH_Uninitialize() { return MH_OK; }
inline MH_STATUS MH_CreateHook(LPVOID pTarget, LPVOID pDetour, LPVOID* ppOriginal) {
    (void)pTarget; (void)pDetour;
    if (ppOriginal) *ppOriginal = nullptr;
    return MH_OK;
}
inline MH_STATUS MH_EnableHook(LPVOID pTarget) { (void)pTarget; return MH_OK; }
inline MH_STATUS MH_DisableHook(LPVOID pTarget) { (void)pTarget; return MH_OK; }
inline MH_STATUS MH_RemoveHook(LPVOID pTarget) { (void)pTarget; return MH_OK; }

#define MH_ALL_HOOKS NULL
