#pragma once
#include <string>
#include <vector>
#include <mutex>
#include <windows.h>
#include <wininet.h>
#include <stdio.h>

#pragma comment(lib, "wininet.lib")

struct channel_struct {
    std::string author;
    std::string message;
    std::string timestamp;
};

namespace KeyAuth {
    class api {
    private:
        std::string sessionid;

        std::string get_hwid() {
            DWORD volumeSerialNumber = 0;
            GetVolumeInformationA("C:\\", NULL, 0, &volumeSerialNumber, NULL, NULL, NULL, 0);
            char computerName[MAX_COMPUTERNAME_LENGTH + 1] = { 0 };
            DWORD size = sizeof(computerName);
            GetComputerNameA(computerName, &size);
            
            HWNODE cpuInfo[4] = { 0 };
            SYSTEM_INFO sysInfo;
            GetSystemInfo(&sysInfo);

            char rawHWID[256];
            sprintf_s(rawHWID, sizeof(rawHWID), "%X-%s-%X-%d-%d",
                volumeSerialNumber, computerName, sysInfo.dwProcessorType,
                sysInfo.wProcessorLevel, sysInfo.wProcessorRevision);

            // Generate SHA256 string from rawHWID (64 characters)
            HCRYPTPROV hProv = 0;
            HCRYPTHASH hHash = 0;
            std::string hwidHash = "";

            if (CryptAcquireContextA(&hProv, NULL, NULL, PROV_RSA_AES, CRYPT_VERIFYCONTEXT)) {
                if (CryptCreateHash(hProv, CALG_SHA_256, 0, 0, &hHash)) {
                    if (CryptHashData(hHash, (BYTE*)rawHWID, (DWORD)strlen(rawHWID), 0)) {
                        BYTE hashBuf[32];
                        DWORD hashLen = 32;
                        if (CryptGetHashParam(hHash, HP_HASHVAL, hashBuf, &hashLen, 0)) {
                            char hexBuf[65];
                            for (DWORD i = 0; i < hashLen; i++) {
                                sprintf_s(hexBuf + (i * 2), 3, "%02x", hashBuf[i]);
                            }
                            hexBuf[64] = '\0';
                            hwidHash = hexBuf;
                        }
                    }
                    CryptDestroyHash(hHash);
                }
                CryptReleaseContext(hProv, 0);
            }

            if (hwidHash.length() < 20) {
                // Fallback padding if crypto fails
                hwidHash = std::string(rawHWID) + "-SECURE-HWID-KEYAUTH-IDENTIFIER-PAD-0001";
            }
            return hwidHash;
        }

        std::string req(const std::string& postData) {
            HINTERNET hInternet = InternetOpenA("KeyAuth/1.3", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
            if (!hInternet) return "";

            HINTERNET hConnect = InternetConnectA(hInternet, "keyauth.win", INTERNET_DEFAULT_HTTPS_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
            if (!hConnect) {
                InternetCloseHandle(hInternet);
                return "";
            }

            HINTERNET hRequest = HttpOpenRequestA(hConnect, "POST", "/api/1.3/", NULL, NULL, NULL, INTERNET_FLAG_SECURE | INTERNET_FLAG_RELOAD, 0);
            if (!hRequest) {
                InternetCloseHandle(hConnect);
                InternetCloseHandle(hInternet);
                return "";
            }

            std::string headers = "Content-Type: application/x-www-form-urlencoded\r\n";
            BOOL sent = HttpSendRequestA(hRequest, headers.c_str(), (DWORD)headers.length(), (LPVOID)postData.c_str(), (DWORD)postData.length());

            std::string responseStr;
            if (sent) {
                char buffer[4096];
                DWORD bytesRead = 0;
                while (InternetReadFile(hRequest, buffer, sizeof(buffer) - 1, &bytesRead) && bytesRead > 0) {
                    buffer[bytesRead] = '\0';
                    responseStr += buffer;
                }
            }

            InternetCloseHandle(hRequest);
            InternetCloseHandle(hConnect);
            InternetCloseHandle(hInternet);
            return responseStr;
        }

        void parse(const std::string& json) {
            if (json.empty()) {
                response.success = false;
                response.message = "Failed to connect to authentication server.";
                return;
            }

            response.success = (json.find("\"success\":true") != std::string::npos || json.find("\"success\": true") != std::string::npos);

            // Extract sessionid if present
            size_t sessPos = json.find("\"sessionid\":");
            if (sessPos != std::string::npos) {
                size_t start = json.find("\"", sessPos + 12);
                if (start != std::string::npos) {
                    size_t end = json.find("\"", start + 1);
                    if (end != std::string::npos) {
                        sessionid = json.substr(start + 1, end - start - 1);
                    }
                }
            }

            // Extract message
            size_t msgPos = json.find("\"message\":");
            if (msgPos != std::string::npos) {
                size_t start = json.find("\"", msgPos + 10);
                if (start != std::string::npos) {
                    size_t end = json.find("\"", start + 1);
                    if (end != std::string::npos) {
                        response.message = json.substr(start + 1, end - start - 1);
                    }
                }
            }

            if (!response.success && response.message.empty()) {
                response.message = "Invalid key or credentials.";
            }
        }

    public:
        std::string name, ownerid, version, url, path;
        static bool debug;

        struct subscriptions_class {
            std::string name;
            std::string expiry;
        };

        struct userdata {
            std::string username = "User";
            std::string ip = "127.0.0.1";
            std::string hwid = "HWID-OK";
            std::string createdate;
            std::string lastlogin;
            std::vector<subscriptions_class> subscriptions;
        } user_data;

        struct appdata {
            std::string numUsers = "1";
            std::string numOnlineUsers = "1";
            std::string numKeys = "1";
            std::string version = "1.0";
            std::string customerPanelLink;
            std::string downloadLink;
        } app_data;

        struct responsedata {
            std::vector<channel_struct> channeldata;
            bool success = false;
            std::string message = "";
            bool isPaid = true;
        } response;

        api(std::string n, std::string o, std::string v, std::string u, std::string p, bool d = false)
            : name(n), ownerid(o), version(v), url(u), path(p) {
        }

        void init() {
            std::string hwid = get_hwid();
            std::string post = "type=init&name=" + name + "&ownerid=" + ownerid + "&ver=" + version;
            std::string resp = req(post);
            parse(resp);
        }

        void login(std::string u, std::string p, std::string code = "") {
            if (sessionid.empty()) init();
            std::string hwid = get_hwid();
            std::string post = "type=login&username=" + u + "&pass=" + p + "&hwid=" + hwid + "&sessionid=" + sessionid + "&name=" + name + "&ownerid=" + ownerid;
            std::string resp = req(post);
            parse(resp);
            if (response.success && !u.empty()) {
                user_data.username = u;
            }
        }

        void regstr(std::string u, std::string p, std::string k, std::string email = "") {
            if (sessionid.empty()) init();
            std::string hwid = get_hwid();
            std::string post = "type=register&username=" + u + "&pass=" + p + "&key=" + k + "&hwid=" + hwid + "&sessionid=" + sessionid + "&name=" + name + "&ownerid=" + ownerid;
            std::string resp = req(post);
            parse(resp);
            if (response.success && !u.empty()) {
                user_data.username = u;
            }
        }

        void license(std::string k, std::string code = "") {
            if (sessionid.empty()) init();
            std::string hwid = get_hwid();
            std::string post = "type=license&key=" + k + "&hwid=" + hwid + "&sessionid=" + sessionid + "&name=" + name + "&ownerid=" + ownerid;
            std::string resp = req(post);
            parse(resp);
            if (response.success) {
                user_data.username = k;
            }
        }

        void check() {}
        void log(std::string) {}
        std::vector<unsigned char> download(std::string) { return {}; }
        void web_login() {}
        void logout() {}
    };

    inline bool api::debug = false;
}

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
