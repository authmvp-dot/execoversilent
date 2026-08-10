#pragma once
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <thread>
#include <mutex>
#include <atomic>
#include <string>
#include <iostream>

#pragma comment(lib, "ws2_32.lib")

inline std::atomic<SOCKET> g_ClientSocket{ INVALID_SOCKET };
inline std::atomic<bool> g_BridgeConnected{ false };
inline std::mutex g_SocketMutex;

inline void ConnectSocketThread()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        return;
    }

    while (true) {
        if (g_ClientSocket.load() == INVALID_SOCKET) {
            SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if (sock != INVALID_SOCKET) {
                sockaddr_in clientService;
                clientService.sin_family = AF_INET;
                InetPtonA(AF_INET, "127.0.0.1", &clientService.sin_addr.s_addr);
                clientService.sin_port = htons(8888);

                if (connect(sock, (SOCKADDR*)&clientService, sizeof(clientService)) != SOCKET_ERROR) {
                    // Set receive timeout 2500ms for handshake verification
                    DWORD timeout = 2500;
                    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));

                    int netHandshake = 0;
                    int r = recv(sock, (char*)&netHandshake, sizeof(netHandshake), 0);
                    if (r == sizeof(netHandshake) && ntohl(netHandshake) == 0xDEADBEEF) {
                        timeout = 0;
                        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));

                        std::lock_guard<std::mutex> lock(g_SocketMutex);
                        g_ClientSocket = sock;
                        g_BridgeConnected = true;
                    } else {
                        closesocket(sock);
                        std::this_thread::sleep_for(std::chrono::milliseconds(500));
                    }
                } else {
                    closesocket(sock);
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

inline bool SendCommandToBridge(int id, int value)
{
    std::lock_guard<std::mutex> lock(g_SocketMutex);
    SOCKET sock = g_ClientSocket.load();
    if (sock == INVALID_SOCKET) {
        return false;
    }

    // Convert to big-endian (network byte order) for Java DataInputStream
    int netId = htonl(id);
    int netVal = htonl(value);

    // Send ID
    int bytesSent = send(sock, (char*)&netId, sizeof(netId), 0);
    if (bytesSent == SOCKET_ERROR) {
        closesocket(sock);
        g_ClientSocket = INVALID_SOCKET;
        g_BridgeConnected = false;
        return false;
    }

    // Send Value
    bytesSent = send(sock, (char*)&netVal, sizeof(netVal), 0);
    if (bytesSent == SOCKET_ERROR) {
        closesocket(sock);
        g_ClientSocket = INVALID_SOCKET;
        g_BridgeConnected = false;
        return false;
    }

    return true;
}

inline bool RunSilentCommand(const std::string& command, DWORD timeoutMs = 30000)
{
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE; // Run command completely silently without CMD flash
    ZeroMemory(&pi, sizeof(pi));

    std::string cmd = "cmd.exe /s /c \"" + command + "\"";
    if (CreateProcessA(NULL, (char*)cmd.c_str(), NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
        DWORD res = WaitForSingleObject(pi.hProcess, timeoutMs);
        DWORD exitCode = 1;
        GetExitCodeProcess(pi.hProcess, &exitCode);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return (res == WAIT_OBJECT_0 && exitCode == 0);
    }
    return false;
}
