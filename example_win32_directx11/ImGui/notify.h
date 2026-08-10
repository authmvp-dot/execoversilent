#define IMGUI_NOTIFY
#define IMGUI_DEFINE_MATH_OPERATORS
#pragma once


#define _WIN32_WINNT 0x0600

#ifndef _WINDOWS_
#include <Windows.h>
#endif

#ifndef GetTickCount64
extern "C" __declspec(dllimport) ULONGLONG WINAPI GetTickCount64(void);
#endif


#include <vector>
#include <string>
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_settings.h>
#include <iostream>
#include <sysinfoapi.h>

extern HWND hwnd;

using namespace ImGui;

#include <thread>
#include <chrono>
#include <random>
using namespace std;
using namespace std::chrono;

static int dismiss = 0;

namespace font
{
    extern ImFont* inter_bold;
    extern ImFont* icomoon;
    extern ImFont* iconuwu;
}

extern ImFont* Montserrat_7;

#define NOTIFY_MAX_MSG_LENGTH			4096		
#define NOTIFY_PADDING_X				15.f		
#define NOTIFY_PADDING_Y				15.f		
#define NOTIFY_PADDING_MESSAGE_Y		15.f		
#define NOTIFY_FADE_IN_OUT_TIME			500
#define NOTIFY_DEFAULT_DISMISS			3000		
#define NOTIFY_OPACITY					1.0f		
#define NOTIFY_TOAST_FLAGS				ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing

#define NOTIFY_INLINE					inline
#define NOTIFY_NULL_OR_EMPTY(str)		(!str ||! strlen(str))
#define NOTIFY_FORMAT(fn, format, ...)	if (format) { va_list args; va_start(args, format); fn(format, args, __VA_ARGS__); va_end(args); }

typedef int ImGuiToastType;
typedef int ImGuiToastPhase;
typedef int ImGuiToastPos;

enum ImGuiToastType_
{
    ImGuiToastType_None,
    ImGuiToastType_Success,
    ImGuiToastType_Warning,
    ImGuiToastType_Error,
    ImGuiToastType_Info,
    ImGuiToastType_Config,
    ImGuiToastType_COUNT
};

enum ImGuiToastPhase_
{
    ImGuiToastPhase_FadeIn,
    ImGuiToastPhase_Wait,
    ImGuiToastPhase_FadeOut,
    ImGuiToastPhase_Expired,
    ImGuiToastPhase_COUNT
};

enum ImGuiToastPos_
{
    ImGuiToastPos_TopLeft,
    ImGuiToastPos_TopCenter,
    ImGuiToastPos_TopRight,
    ImGuiToastPos_BottomLeft,
    ImGuiToastPos_BottomCenter,
    ImGuiToastPos_BottomRight,
    ImGuiToastPos_Center,
    ImGuiToastPos_COUNT
};

class ImGuiToast
{
private:
    ImGuiToastType	type = ImGuiToastType_None;
    char			title[NOTIFY_MAX_MSG_LENGTH];
    char			content[NOTIFY_MAX_MSG_LENGTH];
    int				dismiss_time = NOTIFY_DEFAULT_DISMISS;
    uint64_t		creation_time = 0;

private:
    // Setters

    NOTIFY_INLINE auto set_title(const char* format, va_list args) { vsnprintf(this->title, sizeof(this->title), format, args); }

    NOTIFY_INLINE auto set_content(const char* format, va_list args) { vsnprintf(this->content, sizeof(this->content), format, args); }

public:

    NOTIFY_INLINE int get_dismiss_time() const { return this->dismiss_time; }


    NOTIFY_INLINE auto set_title(const char* format, ...) -> void { NOTIFY_FORMAT(this->set_title, format); }

    NOTIFY_INLINE auto set_content(const char* format, ...) -> void { NOTIFY_FORMAT(this->set_content, format); }

    NOTIFY_INLINE auto set_type(const ImGuiToastType& type) -> void { IM_ASSERT(type < ImGuiToastType_COUNT); this->type = type; };

public:
    // Getters

    NOTIFY_INLINE auto get_title() -> char* { return this->title; };

    NOTIFY_INLINE auto get_default_title() -> const char*
    {
        if (!strlen(this->title))
        {
            switch (this->type)
            {
            case ImGuiToastType_None:
                return NULL;
            case ImGuiToastType_Success:
                return "Success";
            case ImGuiToastType_Warning:
                return "Warning";
            case ImGuiToastType_Error:
                return "Error";
            case ImGuiToastType_Info:
                return "Info";
            case ImGuiToastType_Config:
                return "Config";
            }
        }

        return this->title;
    };



    NOTIFY_INLINE auto get_type() -> const ImGuiToastType& { return this->type; };

    NOTIFY_INLINE auto get_color() -> const ImVec4&
    {
        switch (this->type)
        {




        case ImGuiToastType_None:
            return { c::accent };
        case ImGuiToastType_Success:
            return { c::accent };
        case ImGuiToastType_Warning:
            return { c::accent };
        case ImGuiToastType_Error:
            return { c::accent };
        case ImGuiToastType_Info:
            return { c::accent };
        case ImGuiToastType_Config:
            return { c::accent };

        }
    }

    NOTIFY_INLINE auto get_content() -> char* { return this->content; };

    NOTIFY_INLINE auto get_elapsed_time() { return GetTickCount64() - this->creation_time; }

    NOTIFY_INLINE auto get_phase() -> const ImGuiToastPhase&
    {
        const auto elapsed = get_elapsed_time();

        if (elapsed > NOTIFY_FADE_IN_OUT_TIME + this->dismiss_time + NOTIFY_FADE_IN_OUT_TIME)
        {
            return ImGuiToastPhase_Expired;
        }
        else if (elapsed > NOTIFY_FADE_IN_OUT_TIME + this->dismiss_time)
        {
            return ImGuiToastPhase_FadeOut;
        }
        else if (elapsed > NOTIFY_FADE_IN_OUT_TIME)
        {
            return ImGuiToastPhase_Wait;
        }
        else
        {
            return ImGuiToastPhase_FadeIn;
        }
    }

    NOTIFY_INLINE auto get_fade_percent() -> const float
    {
        const auto phase = get_phase();
        const auto elapsed = get_elapsed_time();

        if (phase == ImGuiToastPhase_FadeIn)
        {
            return ((float)elapsed / (float)NOTIFY_FADE_IN_OUT_TIME) * NOTIFY_OPACITY;
        }
        else if (phase == ImGuiToastPhase_FadeOut)
        {
            return (1.f - (((float)elapsed - (float)NOTIFY_FADE_IN_OUT_TIME - (float)this->dismiss_time) / (float)NOTIFY_FADE_IN_OUT_TIME)) * NOTIFY_OPACITY;
        }

        return 1.f * NOTIFY_OPACITY;
    }

public:

    ImGuiToast(ImGuiToastType type, int dismiss_time = NOTIFY_DEFAULT_DISMISS)
    {
        IM_ASSERT(type < ImGuiToastType_COUNT);

        this->type = type;
        this->dismiss_time = dismiss_time;
        this->creation_time = GetTickCount64();

        memset(this->title, 0, sizeof(this->title));
        memset(this->content, 0, sizeof(this->content));

        dismiss = dismiss_time;

    }

    ImGuiToast(ImGuiToastType type, const char* format, ...) : ImGuiToast(type) { NOTIFY_FORMAT(this->set_content, format); }

    ImGuiToast(ImGuiToastType type, int dismiss_time, const char* format, ...) : ImGuiToast(type, dismiss_time) { NOTIFY_FORMAT(this->set_content, format); }
};

namespace ImGui
{
    NOTIFY_INLINE std::vector<ImGuiToast>& GetNotifications() {
        static std::vector<ImGuiToast> notifications;
        return notifications;
    }

    NOTIFY_INLINE VOID Notification(const ImGuiToast& toast)
    {
        GetNotifications().push_back(toast);
    }

    NOTIFY_INLINE VOID RemoveNotification(int index)
    {
        GetNotifications().erase(GetNotifications().begin() + index);
    }


    NOTIFY_INLINE VOID RenderNotifications()
    {
        if (GetNotifications().empty()) {
            return;
        }

        const auto vp = GetMainViewport();
        const auto vp_pos = vp->Pos;
        const auto vp_size = vp->Size;

        float height = 0.f;

        for (auto i = 0; i < GetNotifications().size(); i++)
        {
            auto* current_toast = &GetNotifications()[i];

            if (current_toast->get_phase() == ImGuiToastPhase_Expired)
            {
                RemoveNotification(i);
                continue;
            }

            const char* title = current_toast->get_title();
            const auto content = current_toast->get_content();
            const auto default_title = current_toast->get_default_title();
            const auto opacity = current_toast->get_fade_percent();
            const auto elapsed = current_toast->get_elapsed_time();
            const auto total_life = NOTIFY_FADE_IN_OUT_TIME * 2 + current_toast->get_dismiss_time();

            float progress = ImClamp((float)elapsed / (float)total_life, 0.0f, 1.0f);

            auto text_color = current_toast->get_color();
            text_color.w = opacity;

            char window_name[50];
            sprintf_s(window_name, "##TOAST%d", i);

            PushStyleVar(ImGuiStyleVar_Alpha, opacity);

            ImVec2 title_size = CalcTextSize(title);
            ImVec2 desc_size = CalcTextSize(content);
            float required_width = 50.f + ImMax(title_size.x, desc_size.x) + 20.f;
            float required_height = 55.f;

            ImVec2 final_pos(0, 0);
            ImGuiWindow* main_window = ImGui::FindWindowByName("##Main");
            if (main_window && !main_window->Hidden) {
                // Attach to the bottom-right of the menu, positioned outside to the right
                final_pos.x = main_window->Pos.x + main_window->Size.x + required_width + 10.f;
                final_pos.y = main_window->Pos.y + main_window->Size.y;
            } else {
                HWND targetHwnd = (::hwnd && IsWindow(::hwnd)) ? ::hwnd : GetForegroundWindow();
                HMONITOR hMon = MonitorFromWindow(targetHwnd, MONITOR_DEFAULTTONEAREST);
                MONITORINFO mi = { sizeof(MONITORINFO) };
                if (GetMonitorInfoA(hMon, &mi)) {
                    final_pos.x = mi.rcWork.right - 15.f;
                    final_pos.y = mi.rcWork.bottom - 15.f;
                }
            }

            SetNextWindowPos(
                ImVec2(final_pos.x, final_pos.y - height),
                ImGuiCond_Always,
                ImVec2(1.f, 1.f)
            );

            PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
            PushStyleVar(ImGuiStyleVar_WindowRounding, 8.f);
            PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);

            PushStyleColor(ImGuiCol_WindowBg, 0);

            Begin(window_name, NULL, NOTIFY_TOAST_FLAGS);
            {
                ImGui::Dummy(ImVec2(required_width, required_height));

                const ImVec2& pos = GetWindowPos();
                const ImVec2& size = GetWindowSize();

                // Draw Background
                GetForegroundDrawList()->AddRectFilled(pos, pos + size, ImColor(18, 18, 18, (int)(opacity * 255)), 8.f);
                GetForegroundDrawList()->AddRect(pos, pos + size, ImColor(40, 40, 40, (int)(opacity * 255)), 8.f);

                // Draw Left Icon
                ImVec2 icon_center = pos + ImVec2(28, size.y / 2.f);
                float icon_radius = 11.f;
                bool is_success = (strcmp(default_title, "Success") == 0);
                ImColor icon_color = is_success ? ImColor(40, 200, 80, (int)(opacity * 255)) : ImColor(200, 40, 40, (int)(opacity * 255));
                
                GetForegroundDrawList()->AddCircle(icon_center, icon_radius, icon_color, 0, 1.5f);
                
                if (is_success) {
                    GetForegroundDrawList()->AddLine(icon_center + ImVec2(-4, 1), icon_center + ImVec2(-1, 4), icon_color, 1.5f);
                    GetForegroundDrawList()->AddLine(icon_center + ImVec2(-1, 4), icon_center + ImVec2(5, -4), icon_color, 1.5f);
                } else {
                    GetForegroundDrawList()->AddLine(icon_center + ImVec2(-3, -3), icon_center + ImVec2(3, 3), icon_color, 1.5f);
                    GetForegroundDrawList()->AddLine(icon_center + ImVec2(3, -3), icon_center + ImVec2(-3, 3), icon_color, 1.5f);
                }

                // Draw Texts
                ImVec2 text_pos = pos + ImVec2(52, 10);
                
                if (font::inter_bold) PushFont(font::inter_bold);
                GetForegroundDrawList()->AddText(text_pos, ImColor(255, 255, 255, (int)(opacity * 255)), title);
                if (font::inter_bold) PopFont();

                ImVec2 desc_pos = text_pos + ImVec2(0, 18);
                GetForegroundDrawList()->AddText(desc_pos, ImColor(150, 150, 150, (int)(opacity * 255)), content);
            }

            height += GetWindowHeight() + 10.f; // 10px spacing between toasts

            End();
            PopStyleVar(3);
            PopStyleColor(1);
        }
    }





}