#pragma once
// ============================================================================
// Keyauth.h  –  Thin wrapper around the REAL KeyAuth C++ SDK (auth/auth.hpp)
// ============================================================================
// The real KeyAuth SDK lives in auth/ and is linked via library_x64.lib.
// This header just re-exports the KeyAuth::api class and sets up the
// KeyAuthClient convenience namespace used by FWorkMain.h.
// ============================================================================

#include "auth/auth.hpp"
#include <mutex>

namespace KeyAuthClient {
    using namespace KeyAuth;

    inline const std::string name = "AIMKILL PVT";
    inline const std::string ownerid = "OrGcs1PvtB";
    inline const std::string version = "1.4";
    inline const std::string url = "https://keyauth.win/api/1.3/";
    inline const std::string path = "";

    inline api Internal(name, ownerid, version, url, path);

    inline std::once_flag init_once_flag;
    inline void EnsureInit() {
        std::call_once(init_once_flag, []() {
            Internal.init();
        });
    }
}
