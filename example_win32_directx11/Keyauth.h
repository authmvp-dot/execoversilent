#pragma once
#include <string>
#include <vector>
#include <mutex>

struct channel_struct {
    std::string author;
    std::string message;
    std::string timestamp;
};

namespace KeyAuth {
    class api {
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
            bool success = true;
            std::string message = "Success";
            bool isPaid = true;
        } response;

        api(std::string n, std::string o, std::string v, std::string u, std::string p, bool d = false)
            : name(n), ownerid(o), version(v), url(u), path(p) {
            response.success = true;
            response.message = "Authenticated";
            user_data.username = "User";
        }

        void init() {
            response.success = true;
            response.message = "Initialized";
        }

        void login(std::string u, std::string, std::string = "") {
            response.success = true;
            response.message = "Login successful";
            user_data.username = u.empty() ? "User" : u;
        }

        void regstr(std::string u, std::string, std::string, std::string = "") {
            response.success = true;
            response.message = "Registration successful";
            user_data.username = u.empty() ? "User" : u;
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

    inline const std::string name = "SILENT AIM X";
    inline const std::string ownerid = "tnktGBPFCy";
    inline const std::string version = "1.0";
    inline const std::string url = "";
    inline const std::string path = "";

    inline api Internal(name, ownerid, version, url, path);

    inline std::once_flag init_once_flag;
    inline void EnsureInit() {
        std::call_once(init_once_flag, []() {
            Internal.init();
        });
    }
}
