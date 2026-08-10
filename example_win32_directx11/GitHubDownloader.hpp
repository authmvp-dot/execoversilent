#pragma once
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <windows.h>
#include <urlmon.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#pragma comment(lib, "urlmon.lib")

// Default GitHub Direct Download URLs for apks_bacend.apk and version.txt
inline std::string g_GitHubApkUrl = "https://github.com/authmvp-dot/apks_bacend34/releases/download/1.0/apks_bacend.apk";
inline std::string g_GitHubVersionUrl = "https://raw.githubusercontent.com/authmvp-dot/apks_bacend34/main/version.txt";

inline std::string GetTempApkFilePath()
{
    char tempPath[MAX_PATH];
    GetTempPathA(MAX_PATH, tempPath);
    return std::string(tempPath) + "apks_bacend.apk";
}

inline std::string GetTempVersionFilePath()
{
    char tempPath[MAX_PATH];
    GetTempPathA(MAX_PATH, tempPath);
    return std::string(tempPath) + "apks_bacend_version.txt";
}

inline std::string GetLocalCachedVersion()
{
    std::ifstream file(GetTempVersionFilePath());
    if (!file.is_open()) return "";
    std::string version;
    std::getline(file, version);
    size_t last = version.find_last_not_of(" \r\n\t");
    if (last != std::string::npos) version = version.substr(0, last + 1);
    return version;
}

inline void SaveLocalVersion(const std::string& version)
{
    std::ofstream file(GetTempVersionFilePath());
    if (file.is_open()) {
        file << version;
    }
}

inline std::string ConvertGitHubToRawUrl(const std::string& inputUrl)
{
    std::string url = inputUrl;
    size_t pos = url.find("github.com/");
    if (pos != std::string::npos) {
        size_t blobPos = url.find("/blob/", pos);
        if (blobPos != std::string::npos) {
            std::string prefix = url.substr(0, pos);
            std::string path = url.substr(pos + 11);
            size_t relBlob = path.find("/blob/");
            if (relBlob != std::string::npos) {
                path.replace(relBlob, 6, "/");
            }
            url = prefix + "raw.githubusercontent.com/" + path;
        }
    }
    return url;
}

inline std::string FetchRemoteVersion(const std::string& versionUrl = "")
{
    std::string rawUrl = ConvertGitHubToRawUrl(versionUrl.empty() ? g_GitHubVersionUrl : versionUrl);
    char tempPath[MAX_PATH];
    GetTempPathA(MAX_PATH, tempPath);
    std::string tempVerFile = std::string(tempPath) + "remote_ver_temp.txt";

    DeleteFileA(tempVerFile.c_str());

    std::string nocacheUrl = rawUrl + "?t=" + std::to_string(GetTickCount());
    HRESULT hr = URLDownloadToFileA(NULL, nocacheUrl.c_str(), tempVerFile.c_str(), 0, NULL);
    if (hr != S_OK) {
        hr = URLDownloadToFileA(NULL, rawUrl.c_str(), tempVerFile.c_str(), 0, NULL);
    }

    std::string remoteVer = "";
    if (hr == S_OK) {
        std::ifstream file(tempVerFile);
        if (file.is_open()) {
            std::getline(file, remoteVer);
            if (remoteVer.find("<!DOCTYPE") != std::string::npos || remoteVer.find("<html") != std::string::npos) {
                remoteVer = "";
            } else {
                size_t last = remoteVer.find_last_not_of(" \r\n\t");
                if (last != std::string::npos) remoteVer = remoteVer.substr(0, last + 1);
            }
        }
        DeleteFileA(tempVerFile.c_str());
    }
    return remoteVer;
}

inline bool DownloadApkFromGitHub(const std::string& downloadUrl = "")
{
    std::string url = downloadUrl.empty() ? g_GitHubApkUrl : downloadUrl;
    std::string destPath = GetTempApkFilePath();

    DeleteFileA(destPath.c_str());

    HRESULT hr = URLDownloadToFileA(NULL, url.c_str(), destPath.c_str(), 0, NULL);
    return (hr == S_OK);
}

inline bool SmartDownloadApkFromGitHub(std::string& outCurrentVersion, const std::string& downloadUrl = "", const std::string& versionUrl = "")
{
    std::string remoteVer = FetchRemoteVersion(versionUrl);
    std::string localVer = GetLocalCachedVersion();
    std::string apkPath = GetTempApkFilePath();

    WIN32_FIND_DATAA findData;
    HANDLE hFind = FindFirstFileA(apkPath.c_str(), &findData);
    bool apkExists = (hFind != INVALID_HANDLE_VALUE);
    if (hFind != INVALID_HANDLE_VALUE) FindClose(hFind);

    if (!remoteVer.empty()) {
        outCurrentVersion = remoteVer;
    } else {
        outCurrentVersion = localVer.empty() ? "1.0" : localVer;
    }

    // If local APK is missing OR remote version is different from cached local version -> DOWNLOAD
    if (!apkExists || remoteVer.empty() || localVer.empty() || remoteVer != localVer) {
        bool downloaded = DownloadApkFromGitHub(downloadUrl);
        if (downloaded) {
            if (!remoteVer.empty()) {
                SaveLocalVersion(remoteVer);
            }
            return true; // Downloaded new or fresh APK
        }
    }

    return false; // Already cached and up to date
}
