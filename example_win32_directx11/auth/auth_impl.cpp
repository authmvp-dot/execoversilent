// ============================================================================
// auth_impl.cpp  –  WinINet-based implementation of KeyAuth::api (auth.hpp)
// ============================================================================
// Replaces library_x64.lib so we don't need libcurl, libsodium, or ATL.
// Uses WinINet for HTTPS POST to https://keyauth.win/api/1.3/
// ============================================================================

#include "auth.hpp"
#include "utils.hpp"
#include "nlohmann/json.hpp"

#include <Windows.h>
#include <wininet.h>
#include <sstream>
#include <ctime>

#pragma comment(lib, "wininet.lib")

using json = nlohmann::json;

namespace KeyAuth {

    bool api::debug = false;

    // ── HTTP helper ──────────────────────────────────────────────────────
    std::string api::req(std::string data, const std::string& url) {
        HINTERNET hNet = InternetOpenA("KeyAuth/1.3", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
        if (!hNet) return "";

        HINTERNET hCon = InternetConnectA(hNet, "keyauth.win", INTERNET_DEFAULT_HTTPS_PORT,
            NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
        if (!hCon) { InternetCloseHandle(hNet); return ""; }

        HINTERNET hReq = HttpOpenRequestA(hCon, "POST", "/api/1.3/", NULL, NULL, NULL,
            INTERNET_FLAG_SECURE | INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE, 0);
        if (!hReq) { InternetCloseHandle(hCon); InternetCloseHandle(hNet); return ""; }

        std::string headers = "Content-Type: application/x-www-form-urlencoded\r\n";
        HttpSendRequestA(hReq, headers.c_str(), (DWORD)headers.length(),
            (LPVOID)data.c_str(), (DWORD)data.length());

        std::string resp;
        char buf[4096];
        DWORD bytesRead = 0;
        while (InternetReadFile(hReq, buf, sizeof(buf) - 1, &bytesRead) && bytesRead > 0) {
            buf[bytesRead] = '\0';
            resp += buf;
        }

        InternetCloseHandle(hReq);
        InternetCloseHandle(hCon);
        InternetCloseHandle(hNet);
        return resp;
    }

    // ── URL-encode ───────────────────────────────────────────────────────
    static std::string url_encode(const std::string& s) {
        std::ostringstream out;
        for (unsigned char c : s) {
            if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~')
                out << c;
            else {
                out << '%';
                out << "0123456789ABCDEF"[c >> 4];
                out << "0123456789ABCDEF"[c & 0x0F];
            }
        }
        return out.str();
    }

    // ── Parse JSON response ──────────────────────────────────────────────
    void api::init() {
        std::string hwid = utils::get_hwid();
        std::string post = "type=init&name=" + url_encode(name) +
            "&ownerid=" + url_encode(ownerid) +
            "&ver=" + url_encode(version);
        std::string resp = req(post, url);

        if (resp.empty()) {
            response.success = false;
            response.message = "Connection failed.";
            return;
        }

        try {
            auto j = json::parse(resp);
            load_response_data(j);
            if (response.success) {
                sessionid = j.value("sessionid", "");
                if (j.contains("appinfo")) {
                    load_app_data(j["appinfo"]);
                }
            }
        }
        catch (...) {
            response.success = false;
            response.message = "Failed to parse server response.";
        }
    }

    void api::license(std::string key, std::string code) {
        std::string hwid = utils::get_hwid();
        std::string post = "type=license&key=" + url_encode(key) +
            "&hwid=" + url_encode(hwid) +
            "&sessionid=" + url_encode(sessionid) +
            "&name=" + url_encode(name) +
            "&ownerid=" + url_encode(ownerid);
        if (!code.empty()) post += "&code=" + url_encode(code);

        std::string resp = req(post, url);
        if (resp.empty()) {
            response.success = false;
            response.message = "Connection failed.";
            return;
        }

        try {
            auto j = json::parse(resp);
            load_response_data(j);
            if (response.success && j.contains("info")) {
                load_user_data(j["info"]);
            }
            if (response.success) {
                mark_authenticated();
            }
        }
        catch (...) {
            response.success = false;
            response.message = "Failed to parse server response.";
        }
    }

    void api::login(std::string username, std::string password, std::string code) {
        std::string hwid = utils::get_hwid();
        std::string post = "type=login&username=" + url_encode(username) +
            "&pass=" + url_encode(password) +
            "&hwid=" + url_encode(hwid) +
            "&sessionid=" + url_encode(sessionid) +
            "&name=" + url_encode(name) +
            "&ownerid=" + url_encode(ownerid);
        if (!code.empty()) post += "&code=" + url_encode(code);

        std::string resp = req(post, url);
        if (resp.empty()) {
            response.success = false;
            response.message = "Connection failed.";
            return;
        }
        try {
            auto j = json::parse(resp);
            load_response_data(j);
            if (response.success && j.contains("info")) {
                load_user_data(j["info"]);
            }
            if (response.success) mark_authenticated();
        }
        catch (...) {
            response.success = false;
            response.message = "Failed to parse server response.";
        }
    }

    void api::regstr(std::string username, std::string password, std::string key, std::string email) {
        std::string hwid = utils::get_hwid();
        std::string post = "type=register&username=" + url_encode(username) +
            "&pass=" + url_encode(password) +
            "&key=" + url_encode(key) +
            "&hwid=" + url_encode(hwid) +
            "&sessionid=" + url_encode(sessionid) +
            "&name=" + url_encode(name) +
            "&ownerid=" + url_encode(ownerid);
        if (!email.empty()) post += "&email=" + url_encode(email);

        std::string resp = req(post, url);
        if (resp.empty()) {
            response.success = false;
            response.message = "Connection failed.";
            return;
        }
        try {
            auto j = json::parse(resp);
            load_response_data(j);
            if (response.success && j.contains("info")) {
                load_user_data(j["info"]);
            }
            if (response.success) mark_authenticated();
        }
        catch (...) {
            response.success = false;
            response.message = "Registration parse error.";
        }
    }

    void api::check(bool check_paid) {
        if (sessionid.empty()) return;
        std::string post = "type=check&sessionid=" + url_encode(sessionid) +
            "&name=" + url_encode(name) + "&ownerid=" + url_encode(ownerid);
        std::string resp = req(post, url);
        try {
            auto j = json::parse(resp);
            load_response_data(j);
        } catch (...) {}
    }

    void api::log(std::string msg) {
        if (sessionid.empty()) return;
        std::string post = "type=log&pcuser=" + url_encode(msg) +
            "&sessionid=" + url_encode(sessionid) +
            "&name=" + url_encode(name) + "&ownerid=" + url_encode(ownerid);
        req(post, url);
    }

    void api::ban(std::string reason) {
        if (sessionid.empty()) return;
        std::string post = "type=ban&sessionid=" + url_encode(sessionid) +
            "&name=" + url_encode(name) + "&ownerid=" + url_encode(ownerid);
        if (!reason.empty()) post += "&reason=" + url_encode(reason);
        req(post, url);
    }

    std::string api::var(std::string varid) {
        if (sessionid.empty()) return "";
        std::string post = "type=var&varid=" + url_encode(varid) +
            "&sessionid=" + url_encode(sessionid) +
            "&name=" + url_encode(name) + "&ownerid=" + url_encode(ownerid);
        std::string resp = req(post, url);
        try {
            auto j = json::parse(resp);
            if (j.value("success", false)) return j.value("message", "");
        } catch (...) {}
        return "";
    }

    std::string api::webhook(std::string id, std::string params, std::string body, std::string contenttype) {
        if (sessionid.empty()) return "";
        std::string post = "type=webhook&webid=" + url_encode(id) +
            "&params=" + url_encode(params) +
            "&sessionid=" + url_encode(sessionid) +
            "&name=" + url_encode(name) + "&ownerid=" + url_encode(ownerid);
        if (!body.empty()) post += "&body=" + url_encode(body);
        if (!contenttype.empty()) post += "&conttype=" + url_encode(contenttype);
        std::string resp = req(post, url);
        try {
            auto j = json::parse(resp);
            return j.value("message", "");
        } catch (...) {}
        return "";
    }

    void api::setvar(std::string v, std::string vardata) {
        if (sessionid.empty()) return;
        std::string post = "type=setvar&var=" + url_encode(v) +
            "&data=" + url_encode(vardata) +
            "&sessionid=" + url_encode(sessionid) +
            "&name=" + url_encode(name) + "&ownerid=" + url_encode(ownerid);
        req(post, url);
    }

    std::string api::getvar(std::string v) {
        if (sessionid.empty()) return "";
        std::string post = "type=getvar&var=" + url_encode(v) +
            "&sessionid=" + url_encode(sessionid) +
            "&name=" + url_encode(name) + "&ownerid=" + url_encode(ownerid);
        std::string resp = req(post, url);
        try {
            auto j = json::parse(resp);
            if (j.value("success", false)) return j.value("response", "");
        } catch (...) {}
        return "";
    }

    bool api::checkblack() {
        if (sessionid.empty()) return false;
        std::string hwid = utils::get_hwid();
        std::string post = "type=checkblacklist&hwid=" + url_encode(hwid) +
            "&sessionid=" + url_encode(sessionid) +
            "&name=" + url_encode(name) + "&ownerid=" + url_encode(ownerid);
        std::string resp = req(post, url);
        try {
            auto j = json::parse(resp);
            return j.value("success", false);
        } catch (...) {}
        return false;
    }

    void api::upgrade(std::string username, std::string key) {
        std::string post = "type=upgrade&username=" + url_encode(username) +
            "&key=" + url_encode(key) +
            "&sessionid=" + url_encode(sessionid) +
            "&name=" + url_encode(name) + "&ownerid=" + url_encode(ownerid);
        std::string resp = req(post, url);
        try {
            auto j = json::parse(resp);
            load_response_data(j);
        } catch (...) {}
    }

    std::vector<unsigned char> api::download(std::string fileid) { return {}; }
    void api::chatget(std::string channel) {}
    bool api::chatsend(std::string message, std::string channel) { return false; }
    void api::changeUsername(std::string newusername) {}
    std::string api::fetchonline() { return ""; }
    void api::fetchstats() {}
    void api::forgot(std::string username, std::string email) {}
    void api::web_login() {}
    void api::button(std::string value) {}

    void api::logout() {
        if (sessionid.empty()) return;
        std::string post = "type=logout&sessionid=" + url_encode(sessionid) +
            "&name=" + url_encode(name) + "&ownerid=" + url_encode(ownerid);
        req(post, url);
        sessionid.clear();
        reset_auth_runtime();
    }

    // ── Ban monitor ──────────────────────────────────────────────────────
    void api::start_ban_monitor(int, bool, std::function<void()>) {}
    void api::stop_ban_monitor() { ban_monitor_running_ = false; }
    bool api::ban_monitor_running() const { return ban_monitor_running_.load(); }
    bool api::ban_monitor_detected() const { return ban_monitor_detected_.load(); }

    // ── Static helpers ───────────────────────────────────────────────────
    std::string api::expiry_remaining(const std::string& expiry) {
        try {
            long long exp = std::stoll(expiry);
            long long now = (long long)std::time(nullptr);
            long long diff = exp - now;
            if (diff <= 0) return "Expired";
            long long days = diff / 86400;
            long long hours = (diff % 86400) / 3600;
            return std::to_string(days) + " days, " + std::to_string(hours) + " hours";
        } catch (...) { return "Unknown"; }
    }

    void api::init_fail_delay() { Sleep(kInitFailSleepMs); }
    void api::bad_input_delay() { Sleep(kBadInputSleepMs); }
    void api::close_delay() { Sleep(kCloseSleepMs); }

    bool api::lockout_active(const lockout_state& state) {
        return state.locked_until > std::chrono::steady_clock::now();
    }

    int api::lockout_remaining_ms(const lockout_state& state) {
        auto remaining = state.locked_until - std::chrono::steady_clock::now();
        int ms = (int)std::chrono::duration_cast<std::chrono::milliseconds>(remaining).count();
        return ms > 0 ? ms : 0;
    }

    void api::record_login_fail(lockout_state& state, int max_attempts, int lock_seconds) {
        state.fails++;
        if (state.fails >= max_attempts) {
            state.locked_until = std::chrono::steady_clock::now() + std::chrono::seconds(lock_seconds);
            state.fails = 0;
        }
    }

    void api::reset_lockout(lockout_state& state) {
        state.fails = 0;
        state.locked_until = {};
    }

    // ── 2FA stubs ────────────────────────────────────────────────────────
    api::Tfa& api::enable2fa(std::string) { return tfa; }
    api::Tfa& api::disable2fa(std::string) { return tfa; }
    api::Tfa& api::Tfa::handleInput(KeyAuth::api&) { return *this; }
    void api::Tfa::QrCode() {}

    // ── Secure strings ──────────────────────────────────────────────────
    void api::enable_secure_strings(bool) {}
    std::string api::get_name() const { return name; }
    std::string api::get_ownerid() const { return ownerid; }
    std::string api::get_version() const { return version; }
    std::string api::get_url() const { return url; }
    std::string api::get_path() const { return path; }
    std::string api::xor_crypt_field(const std::string& in) const { return in; }
    uint32_t api::derive_secure_key() const { return 0; }

    // ── Auth runtime ────────────────────────────────────────────────────
    void api::reset_auth_runtime() {
        auth_nonce_ = 0; auth_window_ = 0; auth_seal_ = 0;
    }
    void api::mark_authenticated() {
        auto now = std::chrono::system_clock::now().time_since_epoch();
        auth_nonce_ = (uint64_t)std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
        auth_window_ = (long long)std::time(nullptr);
        auth_seal_ = compute_auth_seal(auth_nonce_.load(), auth_window_.load());
    }
    void api::refresh_auth_runtime() { mark_authenticated(); }
    bool api::local_auth_valid(bool) const { return auth_seal_ != 0; }
    bool api::has_active_subscription() const {
        return !user_data.subscriptions.empty();
    }
    uint64_t api::compute_auth_seal(uint64_t nonce, long long window) const {
        return nonce ^ (uint64_t)window ^ 0xDEADBEEFCAFE0123ULL;
    }

    // ── Debug ────────────────────────────────────────────────────────────
    void api::debugInfo(std::string, std::string, std::string, std::string) {}
    void api::setDebug(bool value) { debug = value; }
}
