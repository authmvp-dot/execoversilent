#pragma once
#define IMGUI_DEFINE_MATH_OPERATORS

#pragma comment(lib, "libcpmt.lib")

#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "imgui_freetype.h"

#include "ImGui/custom_widgets.hpp"
#include "ImGui/font_defines.h"
#include "ImGui/keybind_system.hpp"
#include "ImGui/TextEditor.h"

#include "Yorzen/Dudas/menu.hpp"

#include "ImGui/SatoshiFont.hpp"

#include <fstream>
#include <streambuf>
#include <shlobj.h>

#include <d3d11.h>
#include <D3DX11.h>
#pragma comment (lib, "d3dx11.lib")

#include "ImGui/image.h"
#include "ImGui/avatar.h"
#include "custom_logo.hpp"
#include "ImGui/custom_popup.hpp"
#include "ImGui/custom_widgets.hpp"
#include <tchar.h>
#include <dwmapi.h>
#pragma comment(lib, "Dwmapi.lib")
#include <ctime>
#include <iomanip>
#include <thread>
#include "ImGui/notify.h"
#include <functional>
#include "Yorzen/Dudas/ui_settings.hpp"
#include "Yorzen/Dudas/menu_ui.h"
#include "Yorzen/Dudas/yorzen.h"
#include "Keyauth.h"
#include "UIStubs.hpp"
#include "src/Overlay/Overlay.hpp"
#include "src/ui/EclipseFontInit.hpp"
#include "esp.h"
#include "src/Globals.hpp"

#include <string>
#include <cstring>
#include <atomic>

static ID3D11Device* g_pd3dDevice = nullptr;
static ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
static IDXGISwapChain* g_pSwapChain = nullptr;
static UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;
static ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;

bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

DWORD picker_flags = ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreview;


bool dark = true;

char field[45] = { "" };

int page = 0;

static float tab_alpha = 0.f; /* */ static float tab_add; /* */ static int active_tab = 0;

int key, m;

bool checkbox_on = true;
bool checkbox_off = false;

static int select1 = 0;
const char* items[3]{ "Selected", "Nope", "what :/ a y?" };

static int bullet_select = 0;
const char* bullet[2]{ "Disabled", "Enabled" };

static int sound_select = 0;
const char* sound[2]{ "Disabled", "Enabled" };

static int style_select = 0;
const char* stylee[2]{ "Flat", "Back" };

static int style2_select = 0;
const char* stylee2[2]{ "Textured", "3D Mode" };

static bool multi_num[5] = { false, true, true, true, false };
const char* multi_items[5] = { "One", "Two", "Three", "Four", "Five" };

float knob = 1.f;

float col[4] = { 118 / 255.f, 187 / 255.f, 117 / 255.f, 0.5f };

//int rotation_start_index;
//void ImRotateStart()
//{
//	rotation_start_index = ImGui::GetWindowDrawList()->VtxBuffer.Size;
//}
//
//ImVec2 ImRotationCenter()
//{
//	ImVec2 l(FLT_MAX, FLT_MAX), u(-FLT_MAX, -FLT_MAX);
//
//	const auto& buf = ImGui::GetWindowDrawList()->VtxBuffer;
//	for (int i = rotation_start_index; i < buf.Size; i++)
//		l = ImMin(l, buf[i].pos), u = ImMax(u, buf[i].pos);
//
//	return ImVec2((l.x + u.x) / 2, (l.y + u.y) / 2);
//}
//
//void ImRotateEnd(float rad, ImVec2 center = ImRotationCenter())
//{
//	float s = sin(rad), c = cos(rad);
//	center = ImRotate(center, s, c) - center;
//
//	auto& buf = ImGui::GetWindowDrawList()->VtxBuffer;
//	for (int i = rotation_start_index; i < buf.Size; i++)
//		buf[i].pos = ImRotate(buf[i].pos, s, c) - center;
//}

void Triangle_background(ImVec2 p) {
	ImVec2 screen_size = c::bg::size;

	static ImVec2 particle_pos[100];
	static ImVec2 particle_speed[100];
	static float particle_size[100];
	static float particle_transparency[100];

	for (int i = 0; i < 100; ++i) {
		if (particle_pos[i].x == 0 && particle_pos[i].y == 0) {
			particle_pos[i].x = rand() % (int)screen_size.x;
			particle_pos[i].y = rand() % 20; // Initial Y position
			particle_speed[i] = ImVec2(rand() % 205, rand() % 205);
			particle_size[i] = rand() % 3 + 3; // Random size
			particle_transparency[i] = static_cast<float>(rand()) / RAND_MAX; // Random transparency
		}

		particle_pos[i] += particle_speed[i] * ImVec2(ImGui::GetIO().DeltaTime, ImGui::GetIO().DeltaTime);

		// ��������� ������������ � ��������� ����
		if (particle_pos[i].x < p.x || particle_pos[i].x > p.x + screen_size.x) {
			// ������ ����������� �� ��� X
			particle_speed[i].x = -particle_speed[i].x;
		}
		if (particle_pos[i].y < p.y || particle_pos[i].y > p.y + screen_size.y + 50) {
			// ������ ����������� �� ��� Y
			particle_speed[i].y = -particle_speed[i].y;
		}

		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		draw_list->AddCircleFilled(particle_pos[i], particle_size[i], ImColor(1.f, 1.f, 1.f, particle_transparency[i]), 16);
		draw_list->AddShadowCircle(particle_pos[i], particle_size[i], ImColor(1.f, 1.f, 1.f, particle_transparency[i]), 30.f, ImVec2(0, 0));
	}
}

namespace ImGui
{

	struct bindbox_cursor
	{
		ImVec2 cursor_pos;
		int mode;
	};

	static inline bool IsKeyDownVK(int vk)
	{
		ImGuiIO& io = ImGui::GetIO();
		if (vk >= 0 && vk < IM_ARRAYSIZE(io.KeysDown))
			return io.KeysDown[vk];
		return false;
	}

	inline void Pickerbox(std::string label, bool* v, float col[4])
	{
		std::string col_label = "color_picker_";
		std::string color_label = col_label + label;

		const ImGuiID id = GetCurrentWindow()->GetID(color_label.c_str());
		static std::map<ImGuiID, bindbox_cursor> anim;
		auto it_anim = anim.find(id);

		if (it_anim == anim.end())
		{
			anim.insert({ id, bindbox_cursor() });
			it_anim = anim.find(id);
		}

		it_anim->second.cursor_pos = GetCursorPos();

		SetCursorPos(it_anim->second.cursor_pos + ImVec2(ImGui::GetContentRegionAvail().x - 90, 4));
		custom::ColorEdit4(label.c_str(), "123", (float*)col, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_PickerHueBar | ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);

		SetCursorPos(it_anim->second.cursor_pos);
		custom::Checkbox(label.c_str(), v);
	}

}


namespace texture
{
	ID3D11ShaderResourceView* preview_slow = nullptr;
	ID3D11ShaderResourceView* avatar_image = nullptr;
	ID3D11ShaderResourceView* logotype_image = nullptr;
	ID3D11ShaderResourceView* background_image = nullptr;
	ID3D11ShaderResourceView* hwnd_image = nullptr;
	extern ID3D11ShaderResourceView* custom_logo;
}

bool CreateDeviceD3D(HWND hWnd)
{

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 2;
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	UINT createDeviceFlags = 0;
	D3D_FEATURE_LEVEL featureLevel;
	const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
	HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
	if (res == DXGI_ERROR_UNSUPPORTED)
		res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
	if (res != S_OK)
		return false;

	CreateRenderTarget();
	return true;
}

void CleanupDeviceD3D()
{
	CleanupRenderTarget();
	if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
	if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
	if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
}

void CreateRenderTarget()
{
	ID3D11Texture2D* pBackBuffer;
	g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
	pBackBuffer->Release();
}

void CleanupRenderTarget()
{
	if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_SIZE:
		if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
		{
			CleanupRenderTarget();
			g_pSwapChain->ResizeBuffers(0, LOWORD(lParam), HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
			CreateRenderTarget();
		}
		return 0;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	}
	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}




namespace markerwall
{
	inline ID3D11ShaderResourceView* Teleenemy = nullptr;
	inline ID3D11ShaderResourceView* Noclip = nullptr;
	inline ID3D11ShaderResourceView* Pull = nullptr;
	inline ID3D11ShaderResourceView* Teleport = nullptr;
	inline ID3D11ShaderResourceView* Parkour = nullptr;
	inline ID3D11ShaderResourceView* Speed = nullptr;
	inline ID3D11ShaderResourceView* Freeze = nullptr;
	inline ID3D11ShaderResourceView* Ghost = nullptr;
	inline ID3D11ShaderResourceView* Keyteclado = nullptr;
}






static bool login_loading = false;
static std::string login_status = "";
static bool login_license_loaded = false;

static std::string GetLoginLicenseFilePath()
{
	char appData[MAX_PATH] = {};
	if (SUCCEEDED(SHGetFolderPathA(nullptr, CSIDL_LOCAL_APPDATA, nullptr, SHGFP_TYPE_CURRENT, appData))) {
		std::string dir = std::string(appData) + "\\BlazeXite\\BRUTAL-X";
		CreateDirectoryA((std::string(appData) + "\\BlazeXite").c_str(), nullptr);
		CreateDirectoryA(dir.c_str(), nullptr);
		return dir + "\\license.key";
	}
	return "license.key";
}

static void LoadLoginLicenseKey()
{
	std::ifstream in(GetLoginLicenseFilePath(), std::ios::binary);
	if (!in)
		return;

	std::string key((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
	while (!key.empty() && (key.back() == '\r' || key.back() == '\n'))
		key.pop_back();
	if (!key.empty())
		strncpy_s(g_Globals.General.License, key.c_str(), _TRUNCATE);
}

static void SaveLoginLicenseKey(const char* key)
{
	if (!key || !key[0])
		return;
	std::ofstream out(GetLoginLicenseFilePath(), std::ios::binary | std::ios::trunc);
	if (out)
		out.write(key, static_cast<std::streamsize>(strlen(key)));
}

static int LicenseKeyInputCallback(ImGuiInputTextCallbackData* data)
{
	if (data->EventFlag == ImGuiInputTextFlags_CallbackEdit) {
		SaveLoginLicenseKey(data->Buf);
		return 0;
	}

	if (data->EventFlag != ImGuiInputTextFlags_CallbackAlways)
		return 0;

	static bool prev_ctrl_v_down = false;
	const bool ctrl_v_down =
		((GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0) &&
		((GetAsyncKeyState('V') & 0x8000) != 0);
	const bool ctrl_v_edge = ctrl_v_down && !prev_ctrl_v_down;
	prev_ctrl_v_down = ctrl_v_down;

	static bool prev_shift_ins_down = false;
	const bool shift_ins_down =
		((GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0) &&
		((GetAsyncKeyState(VK_INSERT) & 0x8000) != 0);
	const bool shift_ins_edge = shift_ins_down && !prev_shift_ins_down;
	prev_shift_ins_down = shift_ins_down;

	if (!ctrl_v_edge && !shift_ins_edge)
		return 0;

	const char* clip = ImGui::GetClipboardText();
	if (!clip)
		clip = "";

	strncpy_s(data->Buf, (size_t)data->BufSize, clip, _TRUNCATE);
	data->BufTextLen = (int)strlen(data->Buf);
	data->BufDirty = true;
	data->CursorPos = data->BufTextLen;
	data->SelectionStart = data->CursorPos;
	data->SelectionEnd = data->CursorPos;
	return 0;
}

#include "Yorzen/Dudas/menu_widgets.hpp"

inline void YorzenRenderPostUI() {}

extern std::atomic<bool> g_AuthDone;
extern std::atomic<bool> g_AuthOK;

namespace YorzenUI {
	inline bool overlay_mode = false;
	inline bool fonts_loaded = false;
	inline bool isClickable = true;

	void InitializeOverlay(HWND overlayHwnd, ID3D11Device* device, ID3D11DeviceContext* ctx);
	void RenderFrame();
	void HandleHotkeys();
}


static ImVec2 next_window_pos = ImVec2(0, 0);


namespace custom {
	bool InputTextWithHint(const char* label, const char* hint, char* buf, size_t buf_size, ImVec2 pos, ImVec2 size,
		ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = nullptr, void* user_data = nullptr)
	{
		ImGui::SetCursorPos(pos);
		return ImGui::InputTextEx(label, hint, buf, (int)buf_size, size, flags, callback, user_data);
	}
}



namespace _Cpp_17 {
	namespace LoginUI {
		constexpr float kWidth = 420.f;
		constexpr float kHeaderH = 75.f;
		constexpr float kPad = 15.f;
		constexpr float kFieldH = 36.f;
		constexpr float kStatusH = 18.f;
		constexpr float kBtnH = 40.f;
		constexpr float kItemGap = 6.f;
		constexpr float kChildW = 390.f;
		// custom::Child inner scroll area height ~= childArgH - 80
		constexpr float kChildArgLogin = 392.f;
		constexpr float kChildArgRegister = 468.f;
		constexpr float kChildOuterLogin = kChildArgLogin - 25.f;
		constexpr float kChildOuterRegister = kChildArgRegister - 25.f;
		constexpr float kBodyTop = kHeaderH + 50.f;
		constexpr float kLoginH = kBodyTop + kChildOuterLogin + 22.f;
		constexpr float kRegisterH = kBodyTop + kChildOuterRegister + 22.f;

		inline float ChildArgForTab(int tab) {
			return (tab == 0) ? kChildArgLogin : kChildArgRegister;
		}
		inline float WindowHForTab(int tab) {
			return (tab == 0) ? kLoginH : kRegisterH;
		}
	}

	static void PushLoginFont(ImFont* f) { if (f) ImGui::PushFont(f); }
	static void PopLoginFont(ImFont* f) { if (f) ImGui::PopFont(); }

	static void EnsureCustomLogoLoaded()
	{
		if (texture::custom_logo || !g_pd3dDevice)
			return;
		D3DX11_IMAGE_LOAD_INFO iInfo{};
		ID3DX11ThreadPump* threadPump = nullptr;
		D3DX11CreateShaderResourceViewFromMemory(
			g_pd3dDevice, custom_logo_bytes, sizeof(custom_logo_bytes), &iInfo, threadPump, &texture::custom_logo, 0);
	}

	static bool LoginInputField(const char* id, const char* label, const char* hint, char* buf, size_t bufSize,
		ImGuiInputTextFlags extraFlags = 0, bool persistLicense = false)
	{
		PushLoginFont(font::medium_small);
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(c::text::label::regular));
		ImGui::TextUnformatted(label);
		ImGui::PopStyleColor();
		PopLoginFont(font::medium_small);

		ImGui::PushID(id);
		ImGui::PushStyleColor(ImGuiCol_FrameBg, c::child::background2);
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.10f, 0.10f, 0.13f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.12f, 0.12f, 0.16f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_Border, c::child::stroke);
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(c::text::label::active));
		ImGui::PushStyleColor(ImGuiCol_TextDisabled, ImVec4(c::text::description::regular));
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, c::elements::rounding);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(14.f, 8.f));
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.f);
		PushLoginFont(font::medium_small);

		const float fieldW = ImGui::GetContentRegionAvail().x;
		const ImGuiInputTextFlags fieldFlags =
			ImGuiInputTextFlags_CallbackAlways |
			(persistLicense ? ImGuiInputTextFlags_CallbackEdit : 0) |
			extraFlags;
		const bool changed = ImGui::InputTextEx("##input", hint, buf, (int)bufSize, ImVec2(fieldW, LoginUI::kFieldH),
			fieldFlags,
			LicenseKeyInputCallback, nullptr);

		if (ImGui::IsItemActive())
			ImGui::GetIO().WantCaptureKeyboard = true;

		PopLoginFont(font::medium_small);
		ImGui::PopStyleVar(3);
		ImGui::PopStyleColor(6);
		ImGui::PopID();
		return changed;
	}

	static void DrawLoginHeader(ImDrawList* draw, ImVec2 pos, float width)
	{
		EnsureCustomLogoLoaded();

		const ImRect header_bb(pos, pos + ImVec2(width, LoginUI::kHeaderH));

		draw->AddRectFilledMultiColor(
			header_bb.Min, header_bb.Max,
			ImColor(36, 8, 8, 255), ImColor(36, 8, 8, 255),
			ImColor(12, 10, 14, 255), ImColor(12, 10, 14, 255));

		draw->PushClipRect(header_bb.Min, header_bb.Max, true);
		shaderrt::Draw(draw, header_bb.Min, header_bb.Max, c::bg::rounding, 0.32f, ImShaderTex_WindowBg);
		draw->PopClipRect();

		ID3D11ShaderResourceView* headerLogo = texture::custom_logo ? texture::custom_logo : texture::logotype_image;
		if (headerLogo) {
			const ImVec2 logoMin = header_bb.Min + ImVec2(12.f, 12.f);
			draw->AddImageRounded(headerLogo, logoMin, logoMin + ImVec2(52.f, 52.f),
				ImVec2(0, 0), ImVec2(1, 1), IM_COL32_WHITE, 360.f);
		}

		PushLoginFont(font::inter_semibold);
		draw->AddText(header_bb.Min + ImVec2(76.f, 16.f), (ImU32)c::main_color, YorzenName::cheat_name);
		PopLoginFont(font::inter_semibold);

		PushLoginFont(font::medium_small);
		draw->AddText(header_bb.Min + ImVec2(76.f, 44.f), ImColor(255, 255, 255, 135), "Secure Access Panel");
		PopLoginFont(font::medium_small);

		draw->AddLine(
			pos + ImVec2(0.f, LoginUI::kHeaderH),
			pos + ImVec2(width, LoginUI::kHeaderH),
			ImGui::GetColorU32(c::child::stroke), 1.f);
	}

	void RenderLoginPage()
	{
		if (!ImGui::GetCurrentContext())
			return;

		EnsureCustomLogoLoaded();

		if (!login_license_loaded) {
			LoadLoginLicenseKey();
			login_license_loaded = true;
		}

		static bool s_loginClickable = false;
		if (!s_loginClickable) {
			ToggleClickability(true);
			s_loginClickable = true;
		}

		static int login_tab = 0;
		static float panel_h = LoginUI::kLoginH;
		static ImVec2 login_window_pos(0.f, 0.f);
		static bool login_window_pos_init = false;

		if (!hTargetWindow && hWindow && IsWindow(hWindow)) {
			RECT currentRect;
			GetWindowRect(hWindow, &currentRect);
			int curW = currentRect.right - currentRect.left;
			int curH = currentRect.bottom - currentRect.top;
			int targetW = (int)LoginUI::kWidth;
			int targetH = (int)panel_h;
			if (curW != targetW || curH != targetH) {
				SetWindowPos(hWindow, NULL, currentRect.left, currentRect.top, targetW, targetH, SWP_NOZORDER | SWP_NOACTIVATE);
			}
			login_window_pos = ImVec2(0.f, 0.f);
		} else {
			const ImVec2 center = ImGui::GetMainViewport()->GetCenter();
			login_window_pos = center - ImVec2(LoginUI::kWidth * 0.5f, panel_h * 0.5f);
		}

		ImGui::SetNextWindowSize(ImVec2(LoginUI::kWidth, panel_h), ImGuiCond_Always);
		ImGui::SetNextWindowPos(login_window_pos, ImGuiCond_Always);

		ImGuiWindowFlags flags =
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoScrollWithMouse |
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoBackground;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.f, 8.f));
		if (!ImGui::Begin("##yorzen_login", nullptr, flags)) {
			ImGui::PopStyleVar(2);
			return;
		}
		{
			const ImVec2 pos = ImGui::GetWindowPos();
			const ImVec2 size = ImGui::GetWindowSize();
			ImDrawList* draw = ImGui::GetWindowDrawList();

			login_window_pos = pos;

			draw->AddRectFilled(pos + ImVec2(0.f, 4.f), pos + size + ImVec2(0.f, 4.f), ImColor(0, 0, 0, 65), c::bg::rounding);
			draw->AddRectFilled(pos, pos + size, c::window_bg_color, c::bg::rounding);
			draw->AddRect(pos, pos + size, ImGui::GetColorU32(c::child::stroke), c::bg::rounding, 0, 1.f);

			DrawLoginHeader(draw, pos, size.x);

			ImGui::SetCursorPos(ImVec2(18.f, LoginUI::kHeaderH + 10.f));
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(14.f, 0.f));
			if (custom::SubTab("Login", &login_tab, 0))
				login_status.clear();
			ImGui::SameLine();
			if (custom::SubTab("Register", &login_tab, 1))
				login_status.clear();
			ImGui::PopStyleVar();

			static float child_arg_h = LoginUI::kChildArgLogin;
			const float target_child_h = LoginUI::ChildArgForTab(login_tab);
			child_arg_h = ImLerp(child_arg_h, target_child_h, ImGui::GetIO().DeltaTime * 14.f);
			if (ImFabs(child_arg_h - target_child_h) < 0.5f)
				child_arg_h = target_child_h;

			ImGui::SetCursorPos(ImVec2(LoginUI::kPad, LoginUI::kHeaderH + 52.f));
			const char* panelTitle = login_tab == 0 ? "Member Sign-In" : "New Registration";
			const char* panelDesc = login_tab == 0 ? "Account Credentials" : "Membership Details";
			const char* panelIcon = login_tab == 0 ? ICON_ENTER_DOOR_LINE : ICON_INVITE_LINE;

			custom::Child(panelTitle, panelIcon, panelDesc, ImVec2(LoginUI::kChildW, child_arg_h), true,
				ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
			{
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.f, LoginUI::kItemGap));

				PushLoginFont(font::s_inter_semibold);
				ImGui::TextColored(ImVec4(c::text::label::active),
					login_tab == 0 ? "Welcome Back" : "Create Your Account");
				PopLoginFont(font::s_inter_semibold);

				PushLoginFont(font::medium_small);
				ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x);
				ImGui::TextColored(ImVec4(c::text::description::regular),
					login_tab == 0
					? "Sign in with your credentials to continue."
					: "Fill in every field to activate your membership.");
				ImGui::PopTextWrapPos();
				PopLoginFont(font::medium_small);

				ImGui::Dummy(ImVec2(0.f, 2.f));

				if (login_tab == 0) {
					LoginInputField("user", "Username", "Your username",
						g_Globals.General.Username, IM_ARRAYSIZE(g_Globals.General.Username));
					LoginInputField("pass", "Password", "Your password",
						g_Globals.General.PassWord, IM_ARRAYSIZE(g_Globals.General.PassWord),
						ImGuiInputTextFlags_Password);
				}
				else {
					LoginInputField("reg_user", "Username", "Choose a username",
						g_Globals.General.Username, IM_ARRAYSIZE(g_Globals.General.Username));
					LoginInputField("reg_pass", "Password", "Choose a password",
						g_Globals.General.PassWord, IM_ARRAYSIZE(g_Globals.General.PassWord),
						ImGuiInputTextFlags_Password);
					LoginInputField("reg_key", "License Key", "Your license key",
						g_Globals.General.License, IM_ARRAYSIZE(g_Globals.General.License), 0, true);
				}

				const bool isError = !login_status.empty() && (
					login_status.find("failed") != std::string::npos ||
					login_status.find("Invalid") != std::string::npos ||
					login_status.find("enter") != std::string::npos ||
					login_status.find("Enter") != std::string::npos ||
					login_status.find("Fill") != std::string::npos ||
					login_status.find("required") != std::string::npos ||
					login_status.find("incorrect") != std::string::npos ||
					login_status.find("Incorrect") != std::string::npos ||
					login_status.find("Could not") != std::string::npos);

				PushLoginFont(font::medium_small);
				ImGui::PushStyleColor(ImGuiCol_Text, isError ? ImVec4(1.f, 0.42f, 0.42f, 1.f)
					: login_status.empty() ? ImVec4(0.f, 0.f, 0.f, 0.f)
					: ImVec4(0.45f, 0.92f, 0.58f, 1.f));
				ImGui::Dummy(ImVec2(ImGui::GetContentRegionAvail().x, LoginUI::kStatusH));
				if (!login_status.empty()) {
					const ImVec2 statusPos = ImGui::GetItemRectMin();
					ImGui::GetWindowDrawList()->AddText(statusPos, ImGui::GetColorU32(ImGuiCol_Text), login_status.c_str());
				}
				ImGui::PopStyleColor();
				PopLoginFont(font::medium_small);

				ImGui::Dummy(ImVec2(0.f, 4.f));
				const char* btnLabel = login_loading
					? "VERIFYING..."
					: (login_tab == 0 ? "SIGN IN" : "CREATE ACCOUNT");
				if (custom::Button(btnLabel, ImVec2(ImGui::GetContentRegionAvail().x, LoginUI::kBtnH)) && !login_loading)
				{
					bool canSubmit = true;
					if (login_tab == 0) {
						if (g_Globals.General.Username[0] == '\0' || g_Globals.General.PassWord[0] == '\0') {
							login_status = "Please enter your username and password.";
							canSubmit = false;
						}
					}
					else if (g_Globals.General.Username[0] == '\0' || g_Globals.General.PassWord[0] == '\0' || g_Globals.General.License[0] == '\0') {
						login_status = "All fields are required to complete registration.";
						canSubmit = false;
					}

					if (canSubmit) {
						login_loading = true;
						login_status = "Verifying credentials...";
						const int tabCopy = login_tab;
						std::string user_copy(g_Globals.General.Username);
						std::string pass_copy(g_Globals.General.PassWord);
						std::string key_copy(g_Globals.General.License);
						std::thread([tabCopy, user_copy, pass_copy, key_copy]() mutable {
							Backend_RunAdbInit();
							if (g_AdbFailed.load()) {
								login_status = "Connection failed. Launch your emulator with Free Fire open.";
								login_loading = false;
								return;
							}

							KeyAuthClient::EnsureInit();
							if (tabCopy == 0)
								KeyAuthClient::Internal.login(user_copy, pass_copy);
							else
								KeyAuthClient::Internal.regstr(user_copy, pass_copy, key_copy);

							if (KeyAuthClient::Internal.response.success) {
								const std::string& user = KeyAuthClient::Internal.user_data.username;
								strncpy_s(var::username, user.c_str(), _TRUNCATE);
								strncpy_s(g_Globals.General.Username, user.c_str(), _TRUNCATE);
								strncpy_s(g_Globals.General.PassWord, pass_copy.c_str(), _TRUNCATE);
								if (!key_copy.empty())
									SaveLoginLicenseKey(key_copy.c_str());
								else if (g_Globals.General.License[0] != '\0')
									SaveLoginLicenseKey(g_Globals.General.License);
								YorzenMain::Auth = true;
								YorzenMain::Login_Window = true;
								login_status = tabCopy == 0 ? "Access granted. Welcome back." : "Registration complete. Welcome aboard.";
								g_AuthOK = true;
								g_AuthDone = true;
							}
							else {
								login_status = KeyAuthClient::Internal.response.message.empty()
									? (tabCopy == 0 ? "Invalid username or password." : "Registration could not be completed.")
									: KeyAuthClient::Internal.response.message;
								YorzenMain::Auth = false;
								g_AuthOK = false;
								g_AuthDone = true;
							}
							login_loading = false;
						}).detach();
					}
				}

				ImGui::PopStyleVar();
			}
			custom::EndChild();
		}
		ImGui::End();
		ImGui::PopStyleVar(2);
	}
}

namespace YorzenUI {

	void InitializeOverlay(HWND overlayHwnd, ID3D11Device* device, ID3D11DeviceContext* ctx)
	{
		if (fonts_loaded)
			return;

		overlay_mode = true;
		hwnd = overlayHwnd;
		g_pd3dDevice = device;
		g_pd3dDeviceContext = ctx;
		g_pSwapChain = FWork::Overlay::dxGetSwapChain();

		IMGUI_CHECKVERSION();
		if (!ImGui::GetCurrentContext())
			ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
		io.IniFilename = nullptr;

		ImFontConfig cfg;
		cfg.FontBuilderFlags = ImGuiFreeTypeBuilderFlags_ForceAutoHint;
		YorzenUI::LoadEclipseFonts(device);

		ImGui_ImplWin32_Init(overlayHwnd);
		ImGui_ImplDX11_Init(device, ctx);

		D3DX11_IMAGE_LOAD_INFO iInfo;
		ID3DX11ThreadPump* threadPump{ nullptr };
		if (texture::avatar_image == nullptr)
			D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, avatar, sizeof(avatar), &iInfo, threadPump, &texture::avatar_image, 0);
		if (texture::custom_logo == nullptr)
			D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, custom_logo_bytes, sizeof(custom_logo_bytes), &iInfo, threadPump, &texture::custom_logo, 0);
		if (texture::logotype_image == nullptr)
			D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, custom_logo_bytes, sizeof(custom_logo_bytes), &iInfo, threadPump, &texture::logotype_image, 0);

		ImGui::StyleColorsLight();
		ImGuiStyle& s = ImGui::GetStyle();
		s.FramePadding = ImVec2(10, 10);
		s.ItemSpacing = ImVec2(0, 0);
		s.FrameRounding = 2.f;
		s.WindowRounding = 10.f;
		s.WindowBorderSize = 0.f;
		s.PopupBorderSize = 0.f;
		s.WindowPadding = ImVec2(10, 10);
		s.ChildBorderSize = 1.f;
		s.Colors[ImGuiCol_Border] = ImVec4(0.f, 0.f, 0.f, 0.f);
		s.Colors[ImGuiCol_BorderShadow] = ImVec4(0.f, 0.f, 0.f, 0.f);
		s.WindowShadowSize = 0;
		s.PopupRounding = 5.f;
		s.ScrollbarSize = 1;

		YorzenMain::Login_Window = true;
		ToggleClickability(true);
		LoadLoginLicenseKey();
		login_license_loaded = true;
		fonts_loaded = true;
	}

	void HandleHotkeys()
	{
		if (GetAsyncKeyState(VK_DELETE) & 0x1)
			exit(0);

		if (GetAsyncKeyState(YorzenKey::ClosedKey) & 0x1)
			exit(0);

		// Menu hide/show is handled by FWork::Interface::HandleMenuKey when overlay_mode is active.
		if (!overlay_mode && GetAsyncKeyState(YorzenKey::HideMenuKey) & 0x1) {
			YorzenMain::Login_Window = !YorzenMain::Login_Window;
			isClickable = YorzenMain::Login_Window;
			ToggleClickability(isClickable);
			if (isClickable) SetForegroundWindow(hwnd);
		}
	}

	void RenderFrame()
	{
		if (!fonts_loaded)
			return;

		g_pSwapChain = FWork::Overlay::dxGetSwapChain();
		g_mainRenderTargetView = FWork::Overlay::dxGetRenderTarget();

		if (!g_pd3dDevice || !g_pd3dDeviceContext)
			return;

		try {
			shaderrt::NewFrame(g_pSwapChain, g_pd3dDevice, g_pd3dDeviceContext, c::main_color);
		}
		catch (const std::exception& e) {
			MessageBoxA(nullptr, e.what(), "Shader Error", MB_ICONERROR | MB_OK);
			exit(1);
		}
		UpdateTheme();

		ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground;
		if (hovered_esp_preview)
			flags |= ImGuiWindowFlags_NoMove;

		if (YorzenMain::Login_Window)
		{
			if (!YorzenMain::Auth)
				_Cpp_17::RenderLoginPage();

			if (YorzenMain::Auth)
			{
				YorzenMain::Login = false;
				ImGui::SetNextWindowSize(ImVec2(c::bg::size.x, c::bg::size.y));
				if (!hTargetWindow && hWindow && IsWindow(hWindow)) {
					RECT currentRect;
					GetWindowRect(hWindow, &currentRect);
					int curW = currentRect.right - currentRect.left;
					int curH = currentRect.bottom - currentRect.top;
					int targetW = (int)c::bg::size.x;
					int targetH = (int)c::bg::size.y;
					if (curW != targetW || curH != targetH) {
						SetWindowPos(hWindow, NULL, currentRect.left, currentRect.top, targetW, targetH, SWP_NOZORDER | SWP_NOACTIVATE);
					}
					ImGui::SetNextWindowPos(ImVec2(0.f, 0.f), ImGuiCond_Always);
				} else {
					ImVec2 menu_center = ImGui::GetMainViewport()->GetCenter() - ImVec2(c::bg::size.x * 0.5f, c::bg::size.y * 0.5f);
					ImGui::SetNextWindowPos(menu_center, ImGuiCond_Always);
				}
				Begin("imgui menu", nullptr, flags);
				{
					ImGuiStyle& s = ImGui::GetStyle();
					c::anim::speed = ImGui::GetIO().DeltaTime * 12.f;

					ImRect main_window_rect(ImGui::GetCurrentWindow()->Rect());
					ImVec2 min = ImGui::GetMainViewport()->Pos;
					ImVec2 max = min + ImGui::GetMainViewport()->Size;
					ImVec2 display_size = ImGui::GetIO().DisplaySize;
					ImDrawList* draw_list = ImGui::GetForegroundDrawList();

					particle::RenderEffects(draw_list, display_size, 24.0f);

					fTabOffset = ImLerp(fTabOffset, bTabState ? 700.f : 0.f, GetAnimSpeed());

					if (fTabOffset > 695.f && bTabState) {
						iTabs = iTabTarget;
						bTabState = false;
					}

					const ImVec2& pos = ImGui::GetWindowPos();
					const ImVec2& region = ImGui::GetContentRegionMax();
					const ImVec2& spacing = s.ItemSpacing;
					ImDrawList* window_draw = ImGui::GetWindowDrawList();
					ImDrawList* bg_draw = ImGui::GetWindowDrawList();

					auto* bg = ImGui::GetBackgroundDrawList();
					ImVec2 bottomRight = pos + c::bg::size;

					bg->AddShadowRect(pos, bottomRight, ImGui::GetColorU32(ImVec4(c::black_color)), 5.0f, ImVec2(0, 0));

					ImGui::PushClipRect(ImVec2(0, 0), ImVec2(4000, 4000), false);
					for (int i = 0; i < 2; i++)
						bg_draw->AddShadowRect(pos, pos + c::bg::size, ImColor(0.f, 0.f, 0.f, 1.f), 250.f, ImVec2(0, 0), ImDrawFlags_RoundCornersAll | ImDrawFlags_ShadowCutOutShapeBackground, c::bg::rounding);
					ImGui::PopClipRect();

					PushFont(font::inter_semibold);
					rainbow.RenderText(utils::center_text(ImGui::GetWindowPos(), ImGui::GetWindowPos() + ImVec2(160, 75), YorzenName::cheat_name), YorzenName::cheat_name, 1.f, 1.f, bg_draw);
					PopFont();

					glow_text_drawlist = GetWindowDrawList();

					static DWORD t = GetTickCount();
					if (GetTickCount() - t > 1000) { blur_reuse = false; t = GetTickCount(); }

					ImGui::PushClipRect(ImVec2(0, 0), ImVec2(4000, 4000), false);

					GetWindowDrawList()->AddRectFilled(pos, pos + c::bg::size, c::window_bg_color, c::bg::rounding);

					// Full-width top header background
					GetWindowDrawList()->AddRectFilled(pos, pos + ImVec2(c::bg::size.x, 75), utils::GetColorWithAlpha(c::child::background, c::child::background.w / 2), c::bg::rounding, ImDrawFlags_RoundCornersTop);
					GetWindowDrawList()->AddRectFilled(pos + ImVec2(0, 75), pos + ImVec2(c::bg::size.x, 76), ImGui::GetColorU32(c::child::stroke), c::bg::rounding);

					// Top-Left Brand Logo Title
					custom::chroma rainbow;
					PushFont(font::inter_semibold);
					rainbow.RenderText(pos + ImVec2(18.f, 24.f), YorzenName::cheat_name, 1.f, 1.f, bg_draw);
					PopFont();

					// Top-Right User Info & Avatar Logo
					{
						ID3D11ShaderResourceView* menuLogo = texture::custom_logo ? texture::custom_logo : texture::avatar_image;
						if (menuLogo)
							GetWindowDrawList()->AddImageRounded(menuLogo, pos + ImVec2(c::bg::size.x - 170.f, 15.f), pos + ImVec2(c::bg::size.x - 125.f, 60.f), ImVec2(0, 0), ImVec2(1, 1), ImColor(1.f, 1.f, 1.f, 1.f), 360.f);
					}

					std::string username = std::string(var::username);
					if (username.empty()) username = "Guest";
					std::string roleKey = "client";

					auto it = userRoles.find(username);
					if (it != userRoles.end()) roleKey = it->second;

					const RoleInfo* rolePtr = nullptr;
					auto roleIt = roleDefinitions.find(roleKey);
					if (roleIt != roleDefinitions.end()) rolePtr = &roleIt->second;
					else rolePtr = &roleDefinitions.at("client");

					const RoleInfo& role = *rolePtr;

					GetWindowDrawList()->AddText(pos + ImVec2(c::bg::size.x - 118.f, 20.f), c::label::active, username.c_str());
					GetWindowDrawList()->AddText(pos + ImVec2(c::bg::size.x - 118.f, 38.f), role.color, role.labelText.c_str());

					ImGui::PopClipRect();

					if (!blur_reuse)
						blur_reuse = true;

					ImGui::SetScrollFromPosY(0);

					// Horizontal Top Header Navigation Tabs
					ImGui::SetCursorPos(ImVec2(120.f, 17.5f));
					ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(6.f, 0.f));
					for (int i = 0; i < IM_ARRAYSIZE(tab_list); i++) {
						if (i > 0) ImGui::SameLine();
						custom::SubTab(tab_list[i], &iTabs, i);
					}
					ImGui::PopStyleVar();

					ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 10));

					// Full-width Tab Content Blocks (Aim / Visual / Brutal / Keybinds / Settings)
					YorzenRenderMenuTabs(fTabOffset);

					ImGui::PopStyleVar();

					window_draw = ImGui::GetWindowDrawList();

					if (YorzenParticle::Enabldparticle)
						ParticlesSpot();
				}
				End();
			}
		}

		ImGui::RenderNotifications();
		Backend_RenderNotifications();
		YorzenRenderPostUI();
	}
}

namespace _Cpp_17 {
	namespace AGDL_Framework {
		int RENDERGUI()
		{
			int width = GetSystemMetrics(SM_CXSCREEN);
			int height = GetSystemMetrics(SM_CYSCREEN);

			WNDCLASSEXW wc = { sizeof(WNDCLASSEXW) };
			wc.style = CS_CLASSDC;
			wc.lpfnWndProc = WndProc;
			wc.cbClsExtra = NULL;
			wc.cbWndExtra = NULL;
			wc.hInstance = nullptr;
			wc.hIcon = LoadIcon(0, IDI_APPLICATION);
			wc.hCursor = LoadCursor(0, IDC_ARROW);
			wc.hbrBackground = nullptr;
			wc.lpszMenuName = nullptr;
			wc.lpszClassName = L" ";
			wc.hIconSm = LoadIcon(0, IDI_APPLICATION);

			RegisterClassExW(&wc);

			hwnd = CreateWindowExW(
				WS_EX_TOPMOST |
				WS_EX_LAYERED |
				WS_EX_TOOLWINDOW |
				WS_EX_APPWINDOW,
				wc.lpszClassName,
				L"",
				WS_POPUP,
				0, 0,
				width,
				height,
				nullptr,
				nullptr,
				wc.hInstance,
				nullptr
			);

			SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);

			ShowWindow(hwnd, SW_SHOW);
			UpdateWindow(hwnd);

			SetForegroundWindow(hwnd);
			SetActiveWindow(hwnd);
			SetFocus(hwnd);

			ToggleClickability(true);
			YorzenMain::Login_Window = true;
			ToggleClickability(YorzenMain::Login_Window);
			if (YorzenMain::Login_Window) SetForegroundWindow(hwnd);


			MARGINS margins = { -1 };
			DwmExtendFrameIntoClientArea(hwnd, &margins);

			POINT mouse;
			RECT rc = { 0 };
			GetWindowRect(hwnd, &rc);

			if (!CreateDeviceD3D(hwnd))
			{
				CleanupDeviceD3D();
				::UnregisterClassW(wc.lpszClassName, wc.hInstance);
			}

			::ShowWindow(hwnd, SW_SHOWDEFAULT);
			::UpdateWindow(hwnd);

			bool isClickable = true;


			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO(); (void)io;
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

			ImFontConfig cfg;
			ckeybind_system pkeybind;
			custom_popup popup("custom popup");

			cfg.FontBuilderFlags = ImGuiFreeTypeBuilderFlags_ForceAutoHint;
			YorzenUI::LoadEclipseFonts(g_pd3dDevice);




			ImGui_ImplWin32_Init(hwnd);
			ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

			bool show_demo_window = true;
			bool show_another_window = false;
			ImVec4 clear_color = ImColor(0, 0, 0, 0);

			D3DX11_IMAGE_LOAD_INFO iInfo;
			ID3DX11ThreadPump* threadPump{ nullptr };
			if (texture::preview_slow == nullptr)
				// D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, preview_slow, sizeof(preview_slow), &iInfo, threadPump, &texture::preview_slow, 0);
				if (texture::avatar_image == nullptr)
					D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, avatar, sizeof(avatar), &iInfo, threadPump, &texture::avatar_image, 0);
			if (texture::custom_logo == nullptr)
				D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, custom_logo_bytes, sizeof(custom_logo_bytes), &iInfo, threadPump, &texture::custom_logo, 0);
			if (texture::logotype_image == nullptr)
				D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, custom_logo_bytes, sizeof(custom_logo_bytes), &iInfo, threadPump, &texture::logotype_image, 0);
			if (texture::background_image == nullptr)
				//	D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, background_img, sizeof(background_img), &iInfo, threadPump, &texture::background_image, 0);
				if (texture::hwnd_image == nullptr)
					//	D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, hwndbackground, sizeof(hwndbackground), &iInfo, threadPump, &texture::hwnd_image, 0);



					//SetForegroundWindow(hwnd);
					//ToggleClickability(true);

					ImGui::StyleColorsLight();
			ImGuiStyle& s = ImGui::GetStyle();
			s.FramePadding = ImVec2(10, 10);
			s.ItemSpacing = ImVec2(0, 0);
			s.FrameRounding = 2.f;
			s.WindowRounding = 10.f;
			s.WindowBorderSize = 0.f;
			s.PopupBorderSize = 0.f;
			s.WindowPadding = ImVec2(10, 10);
			s.ChildBorderSize = 1.f;
			s.Colors[ImGuiCol_Border] = ImVec4(0.f, 0.f, 0.f, 0.f);
			s.Colors[ImGuiCol_BorderShadow] = ImVec4(0.f, 0.f, 0.f, 0.f);
			s.WindowShadowSize = 0;
			s.PopupRounding = 5.f;
			s.ScrollbarSize = 1;

			int msg_count = 0;
			bool done = false;
			while (!done)
			{
				MSG msg;
				while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
				{
					::TranslateMessage(&msg);
					::DispatchMessage(&msg);
					if (msg.message == WM_QUIT)
						done = true;
				}
				if (done)
					break;

				ImGui_ImplDX11_NewFrame();
				ImGui_ImplWin32_NewFrame();

				if (GetAsyncKeyState(VK_DELETE) & 0x1) {
					exit(0);
				}

				if (GetAsyncKeyState(YorzenKey::ClosedKey) & 0x1) {
					exit(0);
				}

				static bool Login_Verified = false;
				if (GetAsyncKeyState(YorzenKey::HideMenuKey) & 0x1) {
					YorzenMain::Login_Window = !YorzenMain::Login_Window;
					isClickable = YorzenMain::Login_Window;
					ToggleClickability(isClickable);
					if (isClickable) SetForegroundWindow(hwnd);
				}

				ImGui::NewFrame();
				SyncUIToGlobals();
				{


					if (EnabledEsp)
					{
					}


					try {
						shaderrt::NewFrame(g_pSwapChain, g_pd3dDevice, g_pd3dDeviceContext, c::main_color);
					}
					catch (const std::exception& e) {
						MessageBoxA(nullptr, e.what(), "Shader Error", MB_ICONERROR | MB_OK);
						exit(1);
					}
					UpdateTheme();

					ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground;
					if (hovered_esp_preview)
						flags |= ImGuiWindowFlags_NoMove;

					if (YorzenMain::Login_Window)
					{
						if (!YorzenMain::Auth)
							_Cpp_17
							::RenderLoginPage();

						if (YorzenMain::Auth)
						{

							YorzenMain::Login = false;
							ImGui::SetNextWindowSize(ImVec2(c::bg::size.x, c::bg::size.y));
							if (next_window_pos.x != 0 && next_window_pos.y != 0)
								ImGui::SetNextWindowPos(next_window_pos, ImGuiCond_Appearing);
							Begin("imgui menu", nullptr, flags);
							{

								c::anim::speed = ImGui::GetIO().DeltaTime * 12.f;

								ImRect main_window_rect(ImGui::GetCurrentWindow()->Rect());
								ImVec2 min = ImGui::GetMainViewport()->Pos;
								ImVec2 max = min + ImGui::GetMainViewport()->Size;
								ImVec2 display_size = ImGui::GetIO().DisplaySize;
								ImDrawList* draw_list = ImGui::GetForegroundDrawList();

								particle::RenderEffects(draw_list, display_size, 24.0f);

								fTabOffset = ImLerp(fTabOffset, bTabState ? 700.f : 0.f, GetAnimSpeed());

								if (fTabOffset > 695.f && bTabState) {
									iTabs = iTabTarget;
									bTabState = false;
								}

								const ImVec2& pos = ImGui::GetWindowPos();
								const ImVec2& region = ImGui::GetContentRegionMax();
								const ImVec2& spacing = s.ItemSpacing;
								ImDrawList* window_draw = ImGui::GetWindowDrawList();
								ImDrawList* bg_draw = ImGui::GetWindowDrawList();

								auto* bg = ImGui::GetBackgroundDrawList();
								ImVec2 bottomRight = pos + c::bg::size;

								bg->AddShadowRect(pos, bottomRight, ImGui::GetColorU32(ImVec4(c::black_color)), 5.0f, ImVec2(0, 0));

								ImGui::PushClipRect(ImVec2(0, 0), ImVec2(4000, 4000), false);
								for (int i = 0; i < 2; i++)
									bg_draw->AddShadowRect(pos, pos + c::bg::size, ImColor(0.f, 0.f, 0.f, 1.f), 250.f, ImVec2(0, 0), ImDrawFlags_RoundCornersAll | ImDrawFlags_ShadowCutOutShapeBackground, c::bg::rounding);
								ImGui::PopClipRect();

								PushFont(font::inter_semibold);
								rainbow.RenderText(utils::center_text(ImGui::GetWindowPos(), ImGui::GetWindowPos() + ImVec2(160, 75), YorzenName::cheat_name), YorzenName::cheat_name, 1.f, 1.f, bg_draw);
								PopFont();

								glow_text_drawlist = GetWindowDrawList();

								static DWORD t = GetTickCount();
								if (GetTickCount() - t > 1000) { blur_reuse = false; t = GetTickCount(); }

								ImGui::PushClipRect(ImVec2(0, 0), ImVec2(4000, 4000), false);

								GetWindowDrawList()->AddRectFilled(pos, pos + c::bg::size, c::window_bg_color, c::bg::rounding);

								GetWindowDrawList()->AddRectFilled(pos + ImVec2(160, 0), pos + ImVec2(c::bg::size.x, 75), utils::GetColorWithAlpha(c::child::background, c::child::background.w / 2), c::bg::rounding, ImDrawFlags_RoundCornersTop);
								GetWindowDrawList()->AddRectFilled(pos, pos + ImVec2(170, c::bg::size.y), ImGui::GetColorU32(c::child::background), c::bg::rounding, ImDrawFlags_RoundCornersLeft);

								GetWindowDrawList()->AddRectFilled(pos + ImVec2(0, 75), pos + ImVec2(c::bg::size.x, 76), ImGui::GetColorU32(c::child::stroke), c::bg::rounding);
								GetWindowDrawList()->AddRectFilled(pos + ImVec2(170, 75), pos + ImVec2(171, c::bg::size.y), ImGui::GetColorU32(c::child::stroke), c::bg::rounding);

								{
									ID3D11ShaderResourceView* menuLogo = texture::custom_logo ? texture::custom_logo : texture::avatar_image;
									if (menuLogo)
										GetWindowDrawList()->AddImageRounded(menuLogo, pos + ImVec2(10, c::bg::size.y - 60), pos + ImVec2(60, c::bg::size.y - 10), ImVec2(0, 0), ImVec2(1, 1), ImColor(1.f, 1.f, 1.f, 1.f), 360.f);
								}

								std::string username = std::string(var::username);
								if (username.empty()) username = "Guest";
								std::string roleKey = "client";

								auto it = userRoles.find(username);
								if (it != userRoles.end()) roleKey = it->second;

								const RoleInfo* rolePtr = nullptr;
								auto roleIt = roleDefinitions.find(roleKey);
								if (roleIt != roleDefinitions.end()) rolePtr = &roleIt->second;
								else rolePtr = &roleDefinitions.at("client");

								const RoleInfo& role = *rolePtr;

								GetWindowDrawList()->AddText(pos + ImVec2(70, c::bg::size.y - 55), c::label::active, username.c_str());
								GetWindowDrawList()->AddText(pos + ImVec2(70, c::bg::size.y - 38), role.color, role.labelText.c_str());

								ImGui::PopClipRect();

								if (!blur_reuse)
									blur_reuse = true;

								ImGui::SetScrollFromPosY(0);

								ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(2, 8));
								ImGui::SetCursorPos(ImVec2(12.5f, 87.5f + ImGui::GetScrollY()));
								ImGui::BeginChild("Tabs", ImVec2(160, c::bg::size.y - 80), false);
								{
									for (int i = 0; i < IM_ARRAYSIZE(tab_list); i++) {
										custom::Tab(tab_list[i], tab_ico_list[i], &iTabs, i);
									}
								}
								ImGui::EndChild(false); ImGui::PopStyleVar();

								ImGui::SetCursorPos(ImVec2(180.f, 17.5f));
								ImGui::Dummy(ImVec2(0.f, 50.f));

								custom::chroma rainbow;
								ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 10));

								YorzenRenderMenuTabs(fTabOffset);

								ImGui::PopStyleVar();

								window_draw = ImGui::GetWindowDrawList();

								PushFont(font::inter_bold);
								ImGui::GetWindowDrawList()->AddText(utils::center_text(ImGui::GetWindowPos(), ImGui::GetWindowPos() + ImVec2(160, 75), YorzenName::cheat_version), utils::GetColorWithAlpha(c::label::regular, 0.5f), YorzenName::cheat_version);
								PopFont();

								PushFont(font::inter_semibold);
								rainbow.RenderText(utils::center_text(ImGui::GetWindowPos(), ImGui::GetWindowPos() + ImVec2(160, 75), YorzenName::cheat_name), YorzenName::cheat_name, 1.f, 1.f, bg_draw);
								PopFont();

								if (YorzenParticle::Enabldparticle)
								{
									ParticlesSpot();
								}

							}
							End();
						}
					}

				}

				ImGui::RenderNotifications();
				Backend_RenderNotifications();
				//SrDudas();

				YorzenRenderPostUI();
				const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
				g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
				g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
				ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

				g_pSwapChain->Present(1, 0);

			}

			ImGui_ImplDX11_Shutdown();
			ImGui_ImplWin32_Shutdown();
			ImGui::DestroyContext();

			CleanupDeviceD3D();
			::DestroyWindow(hwnd);
			::UnregisterClassW(wc.lpszClassName, wc.hInstance);

			return 0;
		}
	}
}
