#include "custom_widgets.hpp"
#pragma comment(lib, "Winmm.lib")
#include <cmath>
#define NOMINMAX
#include <algorithm>
#include <random>
#include <thread>

bool getRandomBool(double probability = 0.5) {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	std::bernoulli_distribution dist(probability);
	return dist(gen);
}

#include "Sounds.hpp"


namespace particle {

	struct Particle {
		float posX, posY, velocityX, velocityY, alpha;
		int lifespan, seed, flag, delay;
		Particle() : posX(0), posY(0), velocityX(0), velocityY(0), alpha(1.0), lifespan(0), seed(0), flag(0), delay(0) {}  // Конструктор по умолчанию
	};

	const int MAX_PARTICLES = 2048;
	Particle particles[MAX_PARTICLES];

	void AddParticle(ImVec2 origin, ImVec2 size, float flag) {
		Particle newParticle;
		newParticle.posX = origin.x + size.x / 2;
		newParticle.posY = origin.y;
		newParticle.velocityX = ((float)rand() / 32767) * 3 - 1.5;
		newParticle.velocityY = ((float)rand() / 32767) * 1.5 - (flag ? 4 : 1);
		newParticle.lifespan = rand() % (flag ? 250 : 299);
		newParticle.seed = rand();
		newParticle.flag = static_cast<int>(flag);
		newParticle.alpha = 1.0;  // Явное указание альфы

		for (int i = 0; i < MAX_PARTICLES; i++) {
			if (particles[i].lifespan == 0) {
				particles[i] = newParticle;
				break;
			}
		}
	}

	void RenderEffects(ImDrawList* drawList, ImVec2 renderSize, float timeOffset) {
		int activeParticles = 0;
		for (int i = 0; i < MAX_PARTICLES; i++) {
			Particle& particle = particles[i];
			if (particle.lifespan) {
				if (particle.delay) {
					particle.delay--;
				}
				else {
					particle.posX += particle.velocityX;
					particle.posY += particle.velocityY;
					particle.velocityY += 0.015;
					particle.lifespan -= (particle.velocityY > 0) ? 1 : 0;

					ImVec2 points[4];  // Массив для точек частицы
					float scale = (static_cast<float>(rand()) / RAND_MAX * 3 + 0.1) * (particle.flag + 1);
					float noise = (timeOffset * (particle.lifespan < 0 ? 0 : 1)) + (i * static_cast<float>(rand()) / RAND_MAX * 2.5 - 1.5);
					float sinAngle = sin(noise) * scale;
					float cosAngle = cos(noise) * scale;

					// Создание "порванных" частиц
					for (int j = 0; j < 4; j++) {
						float angle = 2 * IM_PI * j / 4 + (static_cast<float>(rand()) / RAND_MAX - 0.5) * IM_PI / 8;
						points[j].x = particle.posX + cosAngle * cos(angle) - sinAngle * sin(angle);
						points[j].y = particle.posY + sinAngle * cos(angle) + cosAngle * sin(angle);
					}

					int red = 128 + (rand() % 128);
					int green = 128 + (rand() % 128);
					int blue = 128 + (rand() % 128);



					drawList->AddShadowConvexPoly(points, 4, getRandomBool() ? c::main_color : c::accent_color, 25.f, ImVec2(0, 0));
					drawList->AddConvexPolyFilled(points, 4, getRandomBool() ? c::main_color : c::accent_color);

					if (!particle.lifespan && particle.flag) {

					}
				}
				if (particle.flag)
					activeParticles++;
			}
		}
	}
}


namespace custom
{
	bool GlobalMute = false;
	custom_popup combo_popup("ComboPopup");

	bool IsComboPopupOpen()
	{
		return combo_popup.is_open();
	}

	void ShadeVertsLinearColorGradientKeepAlphaWithAnimation(ImDrawList* draw_list, int vert_start_idx, int vert_end_idx, ImVec2 gradient_p0, ImVec2 gradient_p1, ImU32 col0, ImU32 col1, float time, float speed) {
		float offset = std::fmod(time * speed, 2.0f); // 2.0f для плавного перехода между цветами

		ImVec2 gradient_extent = gradient_p1 - gradient_p0;
		gradient_p0.x += offset * gradient_extent.x;
		gradient_p1.x += offset * gradient_extent.x;

		float gradient_inv_length2 = 1.0f / ImLengthSqr(gradient_extent);
		ImDrawVert* vert_start = draw_list->VtxBuffer.Data + vert_start_idx;
		ImDrawVert* vert_end = draw_list->VtxBuffer.Data + vert_end_idx;

		const int col0_r = (int)(col0 >> IM_COL32_R_SHIFT) & 0xFF;
		const int col0_g = (int)(col0 >> IM_COL32_G_SHIFT) & 0xFF;
		const int col0_b = (int)(col0 >> IM_COL32_B_SHIFT) & 0xFF;

		const int col1_r = (int)(col1 >> IM_COL32_R_SHIFT) & 0xFF;
		const int col1_g = (int)(col1 >> IM_COL32_G_SHIFT) & 0xFF;
		const int col1_b = (int)(col1 >> IM_COL32_B_SHIFT) & 0xFF;

		for (ImDrawVert* vert = vert_start; vert < vert_end; vert++) {
			float d = ImDot(vert->pos - gradient_p0, gradient_extent);
			float t = ImClamp(d * gradient_inv_length2, 0.0f, 1.0f);

			int r = (int)(col0_r + (col1_r - col0_r) * t);
			int g = (int)(col0_g + (col1_g - col0_g) * t);
			int b = (int)(col0_b + (col1_b - col0_b) * t);

			vert->col = (r << IM_COL32_R_SHIFT) | (g << IM_COL32_G_SHIFT) | (b << IM_COL32_B_SHIFT) | (vert->col & IM_COL32_A_MASK);
		}
	}

	int rotation_start_index;


	void ImRotateStart()
	{
		rotation_start_index = ImGui::GetWindowDrawList()->VtxBuffer.Size;
	}

	ImVec2 ImRotationCenter()
	{
		ImVec2 l(FLT_MAX, FLT_MAX), u(-FLT_MAX, -FLT_MAX);

		const auto& buf = ImGui::GetWindowDrawList()->VtxBuffer;
		for (int i = rotation_start_index; i < buf.Size; i++)
			l = ImMin(l, buf[i].pos), u = ImMax(u, buf[i].pos);

		return ImVec2((l.x + u.x) / 2, (l.y + u.y) / 2);
	}

	void ImRotateEnd(float rad, ImVec2 center = ImRotationCenter())
	{
		float s = sin(rad), c = cos(rad);
		center = ImRotate(center, s, c) - center;

		auto& buf = ImGui::GetWindowDrawList()->VtxBuffer;
		for (int i = rotation_start_index; i < buf.Size; i++)
			buf[i].pos = ImRotate(buf[i].pos, s, c) - center;
	}

	struct anim_text_state
	{
		ImVec4 medium_col, semibold_col;
		float medium_offset, semibold_offset;
		float amplitude_power;  // Текущий угол поворота
		float target_rotation_angle;  // Целевой угол поворота
	};

	void draw_volumetric_rect(ImVec2 min, ImVec2 max)
	{
		
		GetWindowDrawList()->AddRectFilled(min, max, c::second_color, c::elements::rounding);
		GetWindowDrawList()->AddRect(min, max, c::stroke_color, c::elements::rounding);

	}

	void RenderAnimatedGradient(const char* text, ImVec2 pos, ImColor color1, ImColor color2, float speed, float time)
	{
		//ImDrawList* draw_list = ImGui::GetWindowDrawList();

		//float offset = std::fmod(time * speed, 1.0f);
		//for (int i = 0; i < strlen(text); i++) {
		//	ImVec2 char_pos = ImVec2(pos.x + i * ImGui::GetFontSize(), pos.y);
		//	float t = std::fmod(offset + static_cast<float>(i) / strlen(text), 1.0f);
		//	ImColor char_color = ImLerpColor(color1, color2, t);
		//	draw_list->AddText(char_pos, char_color, &text[i], &text[i + 1]);
		//}
	}

	void draw_animated_text(ImVec2 pos, ImGuiID id, bool active, bool hovered, const char* text)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();

		window->ID;

		static std::map<ImGuiID, anim_text_state> anim;
		auto it_anim = anim.find(id);

		if (it_anim == anim.end())
		{
			anim.insert({ id, anim_text_state{ ImVec4(1.f, 1.f, 1.f, 1.f), ImVec4(1.f, 1.f, 1.f, 0.f), 0.f, 0.f, 0.f, 0.f } });
			it_anim = anim.find(id);
		}

		float speed = ImGui::GetIO().DeltaTime * 15.f;

		ImColor active_col = active ? label::active : hovered ? label::active : label::regular;

		it_anim->second.medium_col = ImLerp(it_anim->second.medium_col,
			!active ? active_col : utils::GetColorWithAlpha(active_col, 0.f), speed);

		it_anim->second.semibold_col = ImLerp(it_anim->second.semibold_col,
			active ? active_col : utils::GetColorWithAlpha(active_col, 0.f), speed);

		it_anim->second.medium_offset = ImLerp(it_anim->second.medium_offset, active ? -10.f : 0.f, speed);
		it_anim->second.semibold_offset = ImLerp(it_anim->second.semibold_offset, active ? 0.f : 10.f, speed);

		it_anim->second.amplitude_power = ImLerp(it_anim->second.amplitude_power, hovered || active ? (hovered ? 1.5f : 0.f) : 0.f, ImGui::GetIO().DeltaTime * (hovered || active ? 5.f : 2.f));
		ImVec2 medium_pos = ImVec2(pos.x, pos.y + it_anim->second.medium_offset);

		const int vtx_idx_0 = ImGui::GetWindowDrawList()->VtxBuffer.Size;
		const int vtx_idx_1 = glow_text_drawlist->VtxBuffer.Size;

		static float phase = 0.0f;
		static float speed1 = 0.1f;
		static float amplitude = 1.f;

		phase += ImGui::GetIO().DeltaTime * 1.0f;


		PushFont(ImGui::GetDefaultFont());
		ImGui::GetWindowDrawList()->AddText(medium_pos, GetColorU32(it_anim->second.medium_col), text);
		ImGui::GetWindowDrawList()->AddText(medium_pos, GetColorU32(it_anim->second.medium_col), text);

		static float time = 0.0f;

		time += ImGui::GetIO().DeltaTime;

		// Задаём цвета градиента
		ImU32 col0 = IM_COL32(255, 0, 0, 255);   // Красный
		ImU32 col1 = IM_COL32(0, 0, 255, 255);   // Синий

		// Получаем текущий контекст рисования
		ImDrawList* draw_list = ImGui::GetForegroundDrawList();

		// Задаём начальную и конечную точки градиента
		ImVec2 gradient_p0 = ImVec2(50, 50); // Начало градиента
		ImVec2 gradient_p1 = ImVec2(250, 50); // Конец градиента


		ImVec2 text_size = ImGui::CalcTextSize(text);
		ImVec2 text_pos = medium_pos;


		PopFont();


		ImVec2 semibold_pos = ImVec2(pos.x, pos.y + it_anim->second.semibold_offset);

		for (int i = 0; i < glow_context.size(); i++) {
			if (glow_context[i].draw_list != nullptr && glow_context[i].window_id == ImGui::GetCurrentWindow()->ID)
			{
				//glow_context[i].draw_list->AddText(semibold_pos, utils::GetColorWithAlpha(it_anim->second.semibold_col, it_anim->second.semibold_col.w), text);
			}
		}

		ImGui::GetWindowDrawList()->AddText(semibold_pos, GetColorU32(it_anim->second.semibold_col), text);

		const int vtx_idx_2 = ImGui::GetWindowDrawList()->VtxBuffer.Size;
		const int vtx_idx_3 = glow_text_drawlist->VtxBuffer.Size;

		ShadeVertsAnimSine(ImGui::GetWindowDrawList(), vtx_idx_0, vtx_idx_2, phase, speed1, it_anim->second.amplitude_power);

		for (int i = 0; i < glow_context.size(); i++)
			ShadeVertsAnimSine(glow_context[i].draw_list, vtx_idx_1, vtx_idx_3, phase, speed1, it_anim->second.amplitude_power);


	}

	chroma rainbow;


#include <string>

	static const int KEYS_TABLE_LEN = IM_ARRAYSIZE(keys);

	const char* KeyName(int vk)
	{
		static char s_keyNameUtf8[64];
		if (vk <= 0)
			return "None";
		if (vk < KEYS_TABLE_LEN && keys[vk][0] != '-')
			return keys[vk];
		if (vk > 255)
			return "?";
		const UINT scan = MapVirtualKeyW((UINT)vk, MAPVK_VK_TO_VSC);
		if (scan == 0)
			return "?";
		const LONG lp = (LONG)(scan << 16);
		WCHAR wname[64];
		if (GetKeyNameTextW(lp, wname, 64) <= 0)
			return "?";
		if (WideCharToMultiByte(CP_UTF8, 0, wname, -1, s_keyNameUtf8, (int)sizeof(s_keyNameUtf8), nullptr, nullptr) <= 0)
			return "?";
		return s_keyNameUtf8;
	}

	enum KeybindStatus {
		KEYBIND_NONE = 0,
		KEYBIND_WAITING,
		KEYBIND_ASSIGNED
	};

	struct key_state
	{
		ImVec4 background, text, icon;
		bool active = false;
		bool hovered = false;
		float alpha = 0.f;
		float size_x = 50.f;
		int status = KEYBIND_NONE;
		// Prevent the click that enters bind mode from being captured as Mouse1.
		bool clicked_to_waiting = false;
		bool bind_input_armed = false;
	};

	bool Keybind(const char* label, int* key, int* mode)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems) return false;

		ImGuiContext& g = *GImGui;
		ImGuiIO& io = g.IO;
		(void)mode;

		const ImGuiID id = window->GetID(label);
		static ImGuiID s_waiting_id = 0;

		static std::map<ImGuiID, key_state> anim;
		auto it_anim = anim.find(id);
		if (it_anim == anim.end()) {
			anim.insert({ id, key_state() });
			it_anim = anim.find(id);
		}

		// Another keybind took focus — drop out of wait without eating the click.
		if (it_anim->second.status == KEYBIND_WAITING && s_waiting_id != 0 && s_waiting_id != id) {
			it_anim->second.status = (*key != 0) ? KEYBIND_ASSIGNED : KEYBIND_NONE;
			it_anim->second.bind_input_armed = false;
			it_anim->second.clicked_to_waiting = false;
		}

		const bool is_waiting = (it_anim->second.status == KEYBIND_WAITING);
		char buf_display[64] = "None";
		if (is_waiting)
			strcpy_s(buf_display, "...");
		else if (*key != 0)
			strcpy_s(buf_display, KeyName(*key));

		const ImVec2 label_size = CalcTextSize(buf_display, NULL, true);
		const float target_w = ImMax(85.f, label_size.x + 16.f);
		it_anim->second.size_x = ImLerp(it_anim->second.size_x, target_w, io.DeltaTime * 12.f);

		const ImRect rect(window->DC.CursorPos, window->DC.CursorPos + ImVec2(it_anim->second.size_x, 28.f));
		ItemSize(rect);
		if (!ImGui::ItemAdd(rect, id)) return false;

		bool hovered = false;
		bool held = false;
		const bool pressed = ButtonBehavior(rect, id, &hovered, &held);

		it_anim->second.text = ImLerp(it_anim->second.text, is_waiting ? c::text::label::active : hovered ? c::text::label::hovered : c::text::label::regular, GetAnimSpeed());
		it_anim->second.icon = ImLerp(it_anim->second.icon, is_waiting ? c::main_color : hovered ? c::text::label::hovered : c::text::label::regular, GetAnimSpeed());

		GetForegroundDrawList()->PushClipRect(ImGui::GetCurrentWindow()->Rect().Min, ImGui::GetCurrentWindow()->Rect().Max, true);
		PushStyleColor(ImGuiCol_Text, c::text::text_active);
		GetForegroundDrawList()->AddText(utils::center_text(rect.Min, rect.Max, buf_display), c::label::regular, buf_display);
		PopStyleColor();
		GetForegroundDrawList()->PopClipRect();

		// Enter bind mode on press; ignore that same click until mouse is fully released.
		if (pressed) {
			if (it_anim->second.status != KEYBIND_WAITING && !it_anim->second.clicked_to_waiting) {
				s_waiting_id = id;
				it_anim->second.status = KEYBIND_WAITING;
				it_anim->second.clicked_to_waiting = true;
				it_anim->second.bind_input_armed = false;
			}
		}

		if (!io.MouseDown[0])
			it_anim->second.clicked_to_waiting = false;

		bool value_changed = false;

		if (it_anim->second.status == KEYBIND_WAITING && s_waiting_id == id) {
			const bool any_mouse_down =
				io.MouseDown[0] || io.MouseDown[1] || io.MouseDown[2] || io.MouseDown[3] || io.MouseDown[4];
			if (!any_mouse_down)
				it_anim->second.bind_input_armed = true;

			if ((GetAsyncKeyState(VK_ESCAPE) & 0x8000) != 0) {
				*key = 0;
				it_anim->second.status = KEYBIND_NONE;
				it_anim->second.bind_input_armed = false;
				s_waiting_id = 0;
				value_changed = true;
			}
			else if (it_anim->second.bind_input_armed) {
				// Don't steal a click meant for another widget (e.g. switching keybinds).
				const bool click_on_other = ImGui::IsAnyItemHovered() && !hovered;

				for (int i = 0; i < 5 && !click_on_other; i++) {
					if (io.MouseClicked[i]) {
						switch (i) {
						case 0: *key = 0x01; break; // VK_LBUTTON
						case 1: *key = 0x02; break; // VK_RBUTTON
						case 2: *key = 0x04; break; // VK_MBUTTON
						case 3: *key = 0x05; break; // VK_XBUTTON1
						case 4: *key = 0x06; break; // VK_XBUTTON2
						}
						it_anim->second.status = KEYBIND_ASSIGNED;
						it_anim->second.bind_input_armed = false;
						s_waiting_id = 0;
						value_changed = true;
						break;
					}
				}

				if (!value_changed) {
					// Win32 VK codes (matches GetAsyncKeyState checks elsewhere).
					for (int vk = 0x08; vk <= 0xFF; vk++) {
						if (vk == VK_ESCAPE)
							continue;
						if (GetAsyncKeyState(vk) & 0x8000) {
							*key = vk;
							it_anim->second.status = KEYBIND_ASSIGNED;
							it_anim->second.bind_input_armed = false;
							s_waiting_id = 0;
							value_changed = true;
							break;
						}
					}
				}
			}
		}

		if (*key != 0 && it_anim->second.status != KEYBIND_WAITING)
			it_anim->second.status = KEYBIND_ASSIGNED;
		else if (*key == 0 && it_anim->second.status != KEYBIND_WAITING)
			it_anim->second.status = KEYBIND_NONE;

		return value_changed;
	}

	struct child_state
	{
		bool active = true;
		std::string name2;

		ImVec2 label_size;
		ImVec2 desc_size;
		ImVec2 total_text_size;

		float check_delta;
		float size_y;
		float bg_alpha;
		float shader_alpha;
	};

	bool custom::ChildEx(const char* name, const char* icon, const char* description, ImGuiID id, const ImVec2& size_arg, bool cap, ImGuiWindowFlags flags)
	{
		ImGuiContext& g = *GImGui;
		ImGuiWindow* parent_window = g.CurrentWindow;

		flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_ChildWindow;
		flags |= (parent_window->Flags & ImGuiWindowFlags_NoMove);

		static std::map<ImGuiID, child_state> anim;
		auto it_anim = anim.find(id);

		if (it_anim == anim.end())
		{
			anim.insert({ id, child_state() });
			it_anim = anim.find(id);
		}

		if (parent_window->DC.IsSameLine)
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 55);


		float target_size = it_anim->second.active ? size_arg.y - 55 : 30;

		it_anim->second.size_y = ImLerp(it_anim->second.size_y, target_size + (parent_window->DC.IsSameLine ? 55.f : 0.f), GetAnimSpeed());

		const ImVec2 content_avail = GetContentRegionAvail();
		ImVec2 size = ImFloor(ImVec2(size_arg.x, it_anim->second.size_y)) + ImVec2(0, cap ? 30 : 0);
		const int auto_fit_axises = ((size.x == 0.0f) ? (1 << ImGuiAxis_X) : 0x00) | ((size.y == 0.0f) ? (1 << ImGuiAxis_Y) : 0x00);
				

		if (size.x <= 0.0f)
			size.x = ImMax(content_avail.x + size.x, 4.0f);
		if (size.y <= 0.0f)
			size.y = ImMax((content_avail.y) + it_anim->second.size_y, 4.0f);

		float cap_height = 55;

		
		SetNextWindowPos(ImVec2(parent_window->DC.CursorPos + ImVec2(0, cap ? cap_height : 0)));
		SetNextWindowSize(size - ImVec2(0, cap ? cap_height : 0));

		ImRect cap_bb(parent_window->DC.CursorPos, parent_window->DC.CursorPos + ImVec2(size.x, cap_height));
		//ImRect check_bb(ImVec2(cap_bb.Max.x - 50, cap_bb.GetCenter().y - 10), ImVec2(ImVec2(cap_bb.Max.x - 20, cap_bb.GetCenter().y + 10)));

		

		// Фон
		GetWindowDrawList()->AddRectFilled(parent_window->DC.CursorPos, parent_window->DC.CursorPos + size, utils::GetColorWithAlpha(c::child::background, c::child::background.w * it_anim->second.bg_alpha), c::child::rounding, ImDrawFlags_RoundCornersAll);

		GetWindowDrawList()->AddRectFilled(cap_bb.Min, cap_bb.Max, c::second_color, c::child::rounding, ImDrawFlags_RoundCornersTop);

		GetWindowDrawList()->AddRect(parent_window->DC.CursorPos, parent_window->DC.CursorPos + size, GetColorU32(c::child::stroke), c::child::rounding, ImDrawFlags_RoundCornersAll);
		GetWindowDrawList()->AddRectFilled(parent_window->DC.CursorPos + ImVec2(0, cap_height - 1), parent_window->DC.CursorPos + ImVec2(size.x, cap_height), GetColorU32(c::child::stroke), c::child::rounding, ImDrawFlags_RoundCornersTop);

		it_anim->second.check_delta = ImLerp(it_anim->second.check_delta, it_anim->second.active ? 1.f : 0.f, GetAnimSpeed());
		it_anim->second.shader_alpha = ImLerp(it_anim->second.shader_alpha, it_anim->second.active ? 0.65f : 0.1f, GetAnimSpeed());

		//if (check_bb.Contains(ImGui::GetMousePos()) && ImGui::IsMouseClicked(0))
		//	it_anim->second.active = !it_anim->second.active;

		shaderrt::Draw(ImGui::GetWindowDrawList(), cap_bb.Min, cap_bb.Max, c::elements::rounding, 1.f, ImShaderTex_WindowBg);


		//GetWindowDrawList()->AddRectFilled(check_bb.Min, check_bb.Max, GetColorU32(c::window_bg_color.Value), 36);
		//GetWindowDrawList()->AddRectFilled(check_bb.Min, check_bb.Max, utils::GetColorWithAlpha(c::main_color, it_anim->second.check_delta), 36);
		//GetWindowDrawList()->AddRect(check_bb.Min, check_bb.Max, GetColorU32(c::stroke_color.Value), 36);
		
		//GetWindowDrawList()->AddShadowCircle(ImVec2(check_bb.Min.x + 10 + ((check_bb.GetSize().x - 20) * it_anim->second.check_delta), check_bb.GetCenter().y), 5.f, ImColor(0.f, 0.f, 0.f, 0.6f), 15.f, ImVec2(0,0), 0, 36);
		//GetWindowDrawList()->AddCircleFilled(ImVec2(check_bb.Min.x + 10 + ((check_bb.GetSize().x - 20) * it_anim->second.check_delta), check_bb.GetCenter().y), 5.f, ImColor(0.9f, 0.9f, 0.9f, 0.5f + (0.5f * it_anim->second.check_delta)), 36);

		const char* icon_name = nullptr;
		std::string text_name;

		const char* delimiter = strchr(name, '$');
		if (delimiter)
		{
			icon_name = delimiter + 1;
			text_name = std::string(name, delimiter - name);
		}
		else
		{
			text_name = name;
			icon_name = nullptr;
		}
		it_anim->second.total_text_size = ImVec2(it_anim->second.desc_size.x, it_anim->second.desc_size.y + it_anim->second.label_size.y);
		it_anim->second.bg_alpha = ImLerp(it_anim->second.bg_alpha, it_anim->second.active ? ImGui::GetStyle().Alpha : 0.f, ImGui::GetIO().DeltaTime * 7.f);

		ImVec2 label_pos = ImVec2(cap_bb.Min.x + 55.f, cap_bb.GetCenter().y - it_anim->second.total_text_size.y / 2);
		ImVec2 desc_pos = ImVec2(cap_bb.Min.x + 55.f, cap_bb.GetCenter().y);

		PushFont(font::icon_child);
		rainbow.RenderText(ImVec2(parent_window->DC.CursorPos.x + 15.f, utils::center_text(parent_window->DC.CursorPos, parent_window->DC.CursorPos + ImVec2(size.x, cap_height), icon).y), icon, 1.f, 1.f);
		PopFont();

		PushFont(font::s_inter_semibold);
		it_anim->second.label_size = CalcTextSize(text_name.c_str());
		parent_window->DrawList->AddText(label_pos, c::label::active, text_name.c_str());

		PopFont();

		PushFont(font::medium_small);
		it_anim->second.desc_size = CalcTextSize(description);
		parent_window->DrawList->AddText(desc_pos, ImColor(1.f, 1.f, 1.f, 0.5f), description);
		PopFont();

		const char* temp_window_name;
		if (name)
			ImFormatStringToTempBuffer(&temp_window_name, NULL, "%s/%s_%08X", parent_window->Name, name, id);
		else
			ImFormatStringToTempBuffer(&temp_window_name, NULL, "%s/%08X", parent_window->Name, id);

		const float backup_border_size = g.Style.ChildBorderSize;

		int window_flags = ImGuiWindowFlags_NoBackground;
		if (it_anim->second.active)
			window_flags |= ImGuiWindowFlags_NoScrollbar; // Добавляем флаг, если active == true

		bool ret = Begin(temp_window_name, NULL, flags | window_flags);

		ImGuiWindow* child_window = g.CurrentWindow;
		child_window->ChildId = id;
		child_window->AutoFitChildAxises = (ImS8)auto_fit_axises;

		bool found = false;
		for (int i = 0; i < glow_context.size(); i++) {
			if (glow_context[i].window_id == child_window->ID) {
				found = true; // Элемент найден
				break;
			}

			//std::cout << "Name: " << ImGui::FindWindowByID(glow_context[i].window_id)->Name << "value: " << glow_context.size() << std::endl;
		}

		// Если элемент не найден, добавляем его в вектор
		if (!found) {
			glow_context.push_back({ child_window->ParentWindow->DrawList, child_window->ID });
		}

		if (child_window->BeginCount == 1) parent_window->DC.CursorPos = child_window->Pos;

		const ImGuiID temp_id_for_activation = ImHashStr("##Child", 0, id);
		if (g.ActiveId == temp_id_for_activation) ClearActiveID();

		if (g.NavActivateId == id && !(flags & ImGuiWindowFlags_NavFlattened) && (child_window->DC.NavLayersActiveMask != 0 || child_window->DC.NavWindowHasScrollY))
		{
			FocusWindow(child_window);
			NavInitWindow(child_window, false);
			SetActiveID(temp_id_for_activation, child_window);
			g.ActiveIdSource = g.NavInputSource;
		}

		return ret;
	}

	bool custom::Child(const char* str_id, const char* icon, const char* description, const ImVec2& size_arg, bool cap, ImGuiWindowFlags extra_flags)
	{
		ImGuiWindow* window = GetCurrentWindow();

		if (cap) {
			PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(13, 13));
			PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5, 5));
		}
		return ChildEx(str_id, icon, description, window->GetID(str_id), size_arg, cap, extra_flags | ImGuiWindowFlags_AlwaysUseWindowPadding);
	}

	bool custom::ChildID(ImGuiID id, const char* icon, const char* description, const ImVec2& size_arg, bool cap, ImGuiWindowFlags extra_flags)
	{
		IM_ASSERT(id != 0);
		return ChildEx(NULL, icon, description, id, size_arg, cap, extra_flags);
	}

	void EndChild()
	{
		ImGuiContext& g = *GImGui;
		ImGuiWindow* window = g.CurrentWindow;
		PopStyleVar(2);

		IM_ASSERT(g.WithinEndChild == false);
		IM_ASSERT(window->Flags & ImGuiWindowFlags_ChildWindow);

		//ImGui::GetForegroundDrawList()->AddRectFilledMultiColor(p + ImVec2(650, 110), p + ImVec2(760, 460), ImColor(17, 17, 18, 0), ImColor(17, 17, 18, 255), ImColor(17, 17, 18, 255), ImColor(17, 17, 18, 0));

		
		ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImGui::GetWindowPos(), GetWindowPos() + ImVec2(GetWindowSize().x, 15.f), GetColorU32(c::child::background), GetColorU32(c::child::background), utils::GetColorWithAlpha(c::child::background, 0.f), utils::GetColorWithAlpha(c::child::background, 0.f));
		ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImGui::GetWindowPos() + ImVec2(0.f, GetWindowSize().y - 15.f), GetWindowPos() + GetWindowSize(), utils::GetColorWithAlpha(c::child::background, 0.f), utils::GetColorWithAlpha(c::child::background, 0.f), GetColorU32(c::child::background), GetColorU32(c::child::background));



		g.WithinEndChild = true;
		if (window->BeginCount > 1)
		{
			End();
		}
		else
		{
			ImVec2 sz = window->Size;

			if (window->AutoFitChildAxises & (1 << ImGuiAxis_X)) sz.x = ImMax(4.0f, sz.x);
			if (window->AutoFitChildAxises & (1 << ImGuiAxis_Y)) sz.y = ImMax(4.0f, sz.y);

			End();

			ImGuiWindow* parent_window = g.CurrentWindow;
			ImRect bb(parent_window->DC.CursorPos, parent_window->DC.CursorPos + sz);
			ItemSize(sz);
			if ((window->DC.NavLayersActiveMask != 0 || window->DC.NavWindowHasScrollY) && !(window->Flags & ImGuiWindowFlags_NavFlattened))
			{
				ItemAdd(bb, window->ChildId);
			}
			else
			{
				ItemAdd(bb, 0);

				if (window->Flags & ImGuiWindowFlags_NavFlattened) parent_window->DC.NavLayersActiveMaskNext |= window->DC.NavLayersActiveMaskNext;
			}
			if (g.HoveredWindow == window) g.LastItemData.StatusFlags |= ImGuiItemStatusFlags_HoveredWindow;
		}
		g.WithinEndChild = false;
		g.LogLinePosY = -FLT_MAX;
	}


	void BeginGroup()
	{
		ImGuiContext& g = *GImGui;
		ImGuiWindow* window = g.CurrentWindow;

		g.GroupStack.resize(g.GroupStack.Size + 1);
		ImGuiGroupData& group_data = g.GroupStack.back();
		group_data.WindowID = window->ID;
		group_data.BackupCursorPos = window->DC.CursorPos;
		group_data.BackupCursorMaxPos = window->DC.CursorMaxPos;
		group_data.BackupIndent = window->DC.Indent;
		group_data.BackupGroupOffset = window->DC.GroupOffset;
		group_data.BackupCurrLineSize = window->DC.CurrLineSize;
		group_data.BackupCurrLineTextBaseOffset = window->DC.CurrLineTextBaseOffset;
		group_data.BackupActiveIdIsAlive = g.ActiveIdIsAlive;
		group_data.BackupHoveredIdIsAlive = g.HoveredId != 0;
		group_data.BackupActiveIdPreviousFrameIsAlive = g.ActiveIdPreviousFrameIsAlive;
		group_data.EmitItem = true;

		window->DC.GroupOffset.x = window->DC.CursorPos.x - window->Pos.x - window->DC.ColumnsOffset.x;
		window->DC.Indent = window->DC.GroupOffset;
		window->DC.CursorMaxPos = window->DC.CursorPos;
		window->DC.CurrLineSize = ImVec2(0.0f, 0.0f);
		if (g.LogEnabled) g.LogLinePosY = -FLT_MAX;
	}

	void EndGroup()
	{
		ImGuiContext& g = *GImGui;
		ImGuiWindow* window = g.CurrentWindow;
		IM_ASSERT(g.GroupStack.Size > 0);

		ImGuiGroupData& group_data = g.GroupStack.back();
		IM_ASSERT(group_data.WindowID == window->ID);

		if (window->DC.IsSetPos) ErrorCheckUsingSetCursorPosToExtendParentBoundaries();

		ImRect group_bb(group_data.BackupCursorPos, ImMax(window->DC.CursorMaxPos, group_data.BackupCursorPos));

		window->DC.CursorPos = group_data.BackupCursorPos;
		window->DC.CursorMaxPos = ImMax(group_data.BackupCursorMaxPos, window->DC.CursorMaxPos);
		window->DC.Indent = group_data.BackupIndent;
		window->DC.GroupOffset = group_data.BackupGroupOffset;
		window->DC.CurrLineSize = group_data.BackupCurrLineSize;
		window->DC.CurrLineTextBaseOffset = group_data.BackupCurrLineTextBaseOffset;
		if (g.LogEnabled) g.LogLinePosY = -FLT_MAX;

		if (!group_data.EmitItem)
		{
			g.GroupStack.pop_back();
			return;
		}

		window->DC.CurrLineTextBaseOffset = ImMax(window->DC.PrevLineTextBaseOffset, group_data.BackupCurrLineTextBaseOffset);
		ItemSize(group_bb.GetSize());
		ItemAdd(group_bb, 0, NULL, ImGuiItemFlags_NoTabStop);

		const bool group_contains_curr_active_id = (group_data.BackupActiveIdIsAlive != g.ActiveId) && (g.ActiveIdIsAlive == g.ActiveId) && g.ActiveId;
		const bool group_contains_prev_active_id = (group_data.BackupActiveIdPreviousFrameIsAlive == false) && (g.ActiveIdPreviousFrameIsAlive == true);
		if (group_contains_curr_active_id) g.LastItemData.ID = g.ActiveId;
		else if (group_contains_prev_active_id) g.LastItemData.ID = g.ActiveIdPreviousFrame;
		g.LastItemData.Rect = group_bb;

		const bool group_contains_curr_hovered_id = (group_data.BackupHoveredIdIsAlive == false) && g.HoveredId != 0;
		if (group_contains_curr_hovered_id) g.LastItemData.StatusFlags |= ImGuiItemStatusFlags_HoveredWindow;

		if (group_contains_curr_active_id && g.ActiveIdHasBeenEditedThisFrame) g.LastItemData.StatusFlags |= ImGuiItemStatusFlags_Edited;

		g.LastItemData.StatusFlags |= ImGuiItemStatusFlags_HasDeactivated;
		if (group_contains_prev_active_id && g.ActiveId != g.ActiveIdPreviousFrame) g.LastItemData.StatusFlags |= ImGuiItemStatusFlags_Deactivated;

		g.GroupStack.pop_back();
	}




	void Separator_line()
	{
		GetWindowDrawList()->AddRectFilled(GetCursorScreenPos(), GetCursorScreenPos() + ImVec2(GetContentRegionMax().x - GetStyle().WindowPadding.x, 1), GetColorU32(c::separator));
		Spacing();
	}

	void SeparatorEx(ImGuiSeparatorFlags flags, float thickness)
	{
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems) return;

		ImGuiContext& g = *GImGui;
		IM_ASSERT(ImIsPowerOfTwo(flags & (ImGuiSeparatorFlags_Horizontal | ImGuiSeparatorFlags_Vertical)));
		IM_ASSERT(thickness > 0.0f);

		if (flags & ImGuiSeparatorFlags_Vertical)
		{
			float y1 = window->DC.CursorPos.y;
			float y2 = window->DC.CursorPos.y + window->DC.CurrLineSize.y;
			const ImRect bb(ImVec2(window->DC.CursorPos.x, y1 + (GetStyle().ItemSpacing.y / 2)), ImVec2(window->DC.CursorPos.x + thickness, y2 - (GetStyle().ItemSpacing.y / 2)));


			ItemSize(ImVec2(thickness, 0.0f));
			if (!ItemAdd(bb, 0)) return;

			window->DrawList->AddRectFilled(bb.Min, bb.Max, GetColorU32(c::child::background));

			ImGui::SameLine();
		}
		else if (flags & ImGuiSeparatorFlags_Horizontal)
		{
			float x1 = window->Pos.x;
			float x2 = window->Pos.x + window->Size.x;

			if (g.GroupStack.Size > 0 && g.GroupStack.back().WindowID == window->ID) x1 += window->DC.Indent.x;

			if (ImGuiTable* table = g.CurrentTable)
			{
				x1 = table->Columns[table->CurrentColumn].MinX;
				x2 = table->Columns[table->CurrentColumn].MaxX;
			}

			ImGuiOldColumns* columns = (flags & ImGuiSeparatorFlags_SpanAllColumns) ? window->DC.CurrentColumns : NULL;
			if (columns) PushColumnsBackground();

			const float thickness_for_layout = (thickness == 1.0f) ? 0.0f : thickness;
			const ImRect bb(ImVec2(x1 + GetStyle().WindowPadding.x, window->DC.CursorPos.y), ImVec2(x2 - GetStyle().WindowPadding.x, window->DC.CursorPos.y + thickness));

			ItemSize(ImVec2(0.0f, thickness_for_layout));

			if (ItemAdd(bb, 0))
			{
				window->DrawList->AddRectFilled(bb.Min, bb.Max, GetColorU32(c::separator));
			}
			if (columns)
			{
				PopColumnsBackground();
				columns->LineMinY = window->DC.CursorPos.y;
			}
		}
	}

	void Separator()
	{
		ImGuiContext& g = *GImGui;
		ImGuiWindow* window = g.CurrentWindow;
		if (window->SkipItems) return;

		ImGuiSeparatorFlags flags = (window->DC.LayoutType == ImGuiLayoutType_Horizontal) ? ImGuiSeparatorFlags_Vertical : ImGuiSeparatorFlags_Horizontal;
		flags |= ImGuiSeparatorFlags_SpanAllColumns;
		SeparatorEx(flags, 1.0f);
	}

	struct theme_state
	{
		ImVec4 background;
		float smooth_swap, alpha_line, line_size;
	};

	bool ThemeButton(const char* id_theme, bool dark, const ImVec2& size_arg)
	{
		ImGuiWindow* window = GetCurrentWindow();

		if (window->SkipItems) return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(id_theme);
		const ImVec2 label_size = CalcTextSize(id_theme, NULL, true), pos = window->DC.CursorPos;

		ImVec2 size = CalcItemSize(size_arg, label_size.x, label_size.y);

		const ImRect bb(pos, pos + size);

		ItemSize(size, 0.f);
		if (!ItemAdd(bb, id)) return false;

		bool hovered, held, pressed = ButtonBehavior(bb, id, &hovered, &held, NULL);

		static std::map<ImGuiID, theme_state> anim;
		auto it_anim = anim.find(id);

		if (it_anim == anim.end())
		{
			anim.insert({ id, theme_state() });
			it_anim = anim.find(id);
		}

		it_anim->second.background = ImLerp(it_anim->second.background, dark || hovered ? c::page::background_active : c::page::background, g.IO.DeltaTime * 6.f);

		it_anim->second.alpha_line = ImLerp(it_anim->second.alpha_line, dark ? 1.f : 0.f, g.IO.DeltaTime * 6.f);
		it_anim->second.line_size = ImLerp(it_anim->second.line_size, dark ? (size_arg.x / 4) : (size_arg.x / 2), g.IO.DeltaTime * 6.f);

		it_anim->second.smooth_swap = ImLerp(it_anim->second.smooth_swap, dark ? 26.f : 0, g.IO.DeltaTime * 12.f);

		GetWindowDrawList()->AddRectFilled(bb.Min, bb.Max, GetColorU32(it_anim->second.background), c::page::rounding);

		PushClipRect(bb.Min, bb.Max, true);

		PushFont(font::icomoon_page);
		GetWindowDrawList()->AddText(ImVec2(bb.Min.x + (size_arg.x - CalcTextSize("k").x) / 2, bb.Max.y - CalcTextSize("k").y - (size.y - CalcTextSize("k").y) / 2 + it_anim->second.smooth_swap), GetColorU32(c::accent), "k");
		GetWindowDrawList()->AddText(ImVec2(bb.Min.x + (size_arg.x - CalcTextSize("a").x) / 2, bb.Max.y - CalcTextSize("a").y - (size.y - CalcTextSize("a").y) / 2 - 25 + it_anim->second.smooth_swap), GetColorU32(c::accent), "a");
		PopFont();

		PopClipRect();

		return pressed;
	}



	struct button_state
	{
		ImVec4 background, text;
		float shadow_thinkess;
	};

	bool Button(const char* label, const ImVec2& size_arg)
	{
		ImGuiWindow* window = GetCurrentWindow(); 

		if (window->SkipItems) return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);
		const ImVec2 label_size = CalcTextSize(label, NULL, true), pos = window->DC.CursorPos;

		ImVec2 size = CalcItemSize(size_arg, label_size.x, label_size.y);

		const ImRect bb(pos, pos + size);

		ItemSize(size, 0.f);
		if (!ItemAdd(bb, id)) return false;

		bool hovered, held, pressed = ButtonBehavior(bb, id, &hovered, &held, NULL);
		if (combo_popup.is_open())
			pressed = false;

		static std::map<ImGuiID, button_state> anim;
		auto it_anim = anim.find(id);

		if (it_anim == anim.end())
		{
			anim.insert({ id, button_state() });
			it_anim = anim.find(id);
		}

		it_anim->second.background = ImLerp(it_anim->second.background, IsItemActive() || hovered ? c::main_color : c::second_color, g.IO.DeltaTime * 6.f);
		it_anim->second.text = ImLerp(it_anim->second.text, IsItemActive() || hovered ? c::label::active : c::label::regular, g.IO.DeltaTime * 6.f);
		it_anim->second.shadow_thinkess = ImLerp(it_anim->second.shadow_thinkess, IsItemActive() || hovered ? 1.f : 0.f, g.IO.DeltaTime * 6.f);

		shaderrt::Draw(ImGui::GetWindowDrawList(), bb.Min, bb.Max, c::elements::rounding, it_anim->second.shadow_thinkess, ImShaderTex_WindowBg);


		draw_volumetric_rect(bb.Min, bb.Max);


		
		GetWindowDrawList()->AddText(utils::center_text(bb.Min, bb.Max, label), GetColorU32(it_anim->second.text), label);

		return pressed;
	}

	struct tab_state {
		ImVec4 text_col, icon_col;
		ImVec4 frame_col;
		ImVec4 line_col;
		float shadow_thinkess, active_icon_alpha, shader_alpha;
	};

	bool Tab(const char* label, const char* icon, int* v, int number)
	{
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);

		const float square_sz = GetFrameHeight();
		const ImVec2 pos = window->DC.CursorPos;
		const ImRect total_bb(pos, pos + ImVec2(145, 40));


		ItemSize(total_bb, style.FramePadding.y);
		ItemAdd(total_bb, id);

		static std::map<ImGuiID, tab_state> anim;
		auto it_anim = anim.find(id);

		if (it_anim == anim.end())
		{
			anim.insert({ id, tab_state() });
			it_anim = anim.find(id);
		}

		bool hovered, held;
		bool pressed = ButtonBehavior(total_bb, id, &hovered, &held);
		if (pressed)
		{
			bTabState = true;
			iTabTarget = number;
		}

		RenderNavHighlight(total_bb, id);

		it_anim->second.frame_col = ImLerp(it_anim->second.frame_col, *v == number ? c::second_color : utils::GetColorWithAlpha(c::second_color, 0.f), GetAnimSpeed());
		it_anim->second.text_col = ImLerp(it_anim->second.text_col, *v == number ? c::label::active : c::label::regular, GetAnimSpeed());
		it_anim->second.icon_col = ImLerp(it_anim->second.icon_col, *v == number ? c::main_color : c::label::regular, GetAnimSpeed());
		it_anim->second.line_col = ImLerp(it_anim->second.line_col, *v == number ? c::main_color : utils::GetColorWithAlpha(c::main_color, 0.f), GetAnimSpeed());
		it_anim->second.active_icon_alpha = ImLerp(it_anim->second.active_icon_alpha, *v == number ? 1.f : 0.f, GetAnimSpeed());
		it_anim->second.shadow_thinkess = ImLerp(it_anim->second.shadow_thinkess, *v == number ? 1.f : 0.f, GetAnimSpeed());
		it_anim->second.shader_alpha = ImLerp(it_anim->second.shader_alpha, *v == number ? 1.f : 0.f, GetAnimSpeed());

		shaderrt::Draw(ImGui::GetWindowDrawList(), total_bb.Min, total_bb.Max, c::elements::rounding, it_anim->second.shader_alpha, ImShaderTex_WindowBg);

		//window->DrawList->AddRectFilled(total_bb.Min, ImVec2(total_bb.Min.x + 4.f, total_bb.Max.y), GetColorU32(it_anim->second.line_col), style.FrameRounding, ImDrawFlags_RoundCornersLeft);

		ImGui::PushClipRect(ImVec2(0, 0), ImGui::GetMainViewport()->Size, false);
		rainbow.RenderRect(total_bb.Min, ImVec2(total_bb.Min.x + 4.f, total_bb.Max.y), c::elements::rounding, true, it_anim->second.line_col.w, label, it_anim->second.shadow_thinkess);
		rainbow.RenderRect(total_bb.Max - ImVec2(4.f, total_bb.GetSize().y), ImVec2(total_bb.Max.x, total_bb.Max.y), c::elements::rounding, true, it_anim->second.line_col.w, label, it_anim->second.shadow_thinkess);
		ImGui::PopClipRect();

		PushFont(font::icomoon_tabs);
		ImRect icon_bb(ImVec2(total_bb.Min.x + 15.f, utils::center_text(total_bb.Min, total_bb.Max, label).y), ImVec2(total_bb.Min.x + 15.f, utils::center_text(total_bb.Min, total_bb.Max, label).y) + CalcTextSize(icon));

		window->DrawList->AddText(icon_bb.Min, GetColorU32(it_anim->second.icon_col), icon);
		rainbow.RenderText(icon_bb.Min, icon, 1.f, it_anim->second.active_icon_alpha);
		PopFont();

		window->DrawList->AddText(ImVec2(total_bb.Min.x + 48, utils::center_text(total_bb.Min, total_bb.Max, label).y), GetColorU32(it_anim->second.text_col), label);

		return pressed;
	}

	struct check_state
	{
		ImVec4 label_col, description_col, background, mark_col, rect_color, highlighting;
		float check_offset;
		float shader_alpha;
	};

	bool Checkbox(const char* label, bool* v, float right_margin)
	{
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems) return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);
		const ImVec2 label_size = CalcTextSize(label, NULL, true);

		const float square_sz = 10;
		const ImVec2 pos = window->DC.CursorPos;

		const float w = GetContentRegionMax().x - style.WindowPadding.x - right_margin;

		const ImRect total_bb(pos, pos + ImVec2(w, 39));
		const ImRect box_bb(total_bb.Max - ImVec2(total_bb.GetSize().y - 6.f, total_bb.GetSize().y), total_bb.Max - ImVec2(6.f, 0.f));

		ItemSize(total_bb, 0.f);

		if (!ItemAdd(total_bb, id)) return false;

		bool hovered, held, pressed = ButtonBehavior(total_bb, id, &hovered, &held);
		if (combo_popup.is_open())
			pressed = false;

		static std::map<ImGuiID, check_state> anim;
		auto it_anim = anim.emplace(id, check_state()).first;

		std::string label_str = label;

		it_anim->second.label_col = ImLerp(it_anim->second.label_col, *v ? c::accent : hovered ? c::accent : c::accent, GetAnimSpeed());

		it_anim->second.description_col = ImLerp(it_anim->second.description_col, *v ? c::accent : hovered ? c::accent : c::accent, GetAnimSpeed());

		it_anim->second.rect_color = ImLerp(it_anim->second.rect_color, *v ? c::main_color : c::second_color, GetAnimSpeed());

		it_anim->second.mark_col = ImLerp(it_anim->second.mark_col, *v ? utils::GetDarkColor(c::main_color) : utils::GetColorWithAlpha(utils::GetDarkColor(c::main_color), 0.f), GetAnimSpeed() * 3);

		
		it_anim->second.shader_alpha = ImLerp(it_anim->second.shader_alpha, *v ? shader_alpha : 0.f, GetAnimSpeed());

		const ImRect inactive_check_bb(box_bb.GetCenter() - ImVec2(20, 8), box_bb.GetCenter() + ImVec2(8, 8));


		it_anim->second.check_offset = ImLerp(
			it_anim->second.check_offset,
			*v ? 1.0f : 0.0f,
			GetAnimSpeed()
		);

		// позиция кружка считается каждый кадр от bb
		float x = ImLerp(
			inactive_check_bb.Min.x + inactive_check_bb.GetHeight() * 0.5f,
			inactive_check_bb.Max.x - inactive_check_bb.GetHeight() * 0.5f,
			it_anim->second.check_offset
		);

		

		shaderrt::Draw(ImGui::GetWindowDrawList(), total_bb.Min, total_bb.Max, c::elements::rounding, it_anim->second.shader_alpha, ImShaderTex_WindowBg);
			
		if (IsItemClicked() && !combo_popup.is_open())
		{
			*v = !(*v);
			if (!GlobalMute) {
				if (*v) std::thread([](){ Beep(600, 100); }).detach(); // Checked - Higher Pitch - Async
				else std::thread([](){ Beep(400, 100); }).detach();    // Unchecked - Lower Pitch - Async
			}

			MarkItemEdited(id);
		}


		draw_volumetric_rect(total_bb.Min, total_bb.Max);

		GetWindowDrawList()->AddShadowRect(inactive_check_bb.Min, inactive_check_bb.Max, utils::GetColorWithAlpha(it_anim->second.rect_color, shader_alpha), 55.f, ImVec2(0, 0), 36);

		GetWindowDrawList()->AddRect(inactive_check_bb.Min, inactive_check_bb.Max, c::stroke_color, 36);

		GetWindowDrawList()->AddRect(inactive_check_bb.Min, inactive_check_bb.Max, GetColorU32(it_anim->second.rect_color), 36);
		
		
		ImGui::GetWindowDrawList()->AddCircleFilled(
			ImVec2(x, inactive_check_bb.GetCenter().y),
			inactive_check_bb.GetHeight() * 0.5f - 4.0f,
			IM_COL32(255, 255, 255, 255)
		);
		//rainbow.RenderRect(check_bb.Min, check_bb.Max, c::elements::rounding, true, it_anim->second.rect_color.w, label, it_anim->second.shadow_delta);

		
		draw_animated_text(ImVec2(pos.x + 10.5f, utils::center_text(total_bb.Min, total_bb.Max, label).y), id, *v, hovered, label);

		return pressed;
	}

	bool KeyYorzen(const char* label, bool* v)
	{
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems) return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);

		const ImVec2 pos = window->DC.CursorPos;
		const float w = GetContentRegionMax().x - style.WindowPadding.x;

		// Fila/row del ítem
		const ImRect total_bb(pos, pos + ImVec2(w, 39));
		ItemSize(total_bb, 0.f);
		if (!ItemAdd(total_bb, id)) return false;

		bool hovered, held, pressed = ButtonBehavior(total_bb, id, &hovered, &held);

		static std::map<ImGuiID, check_state> anim;
		auto it_anim = anim.emplace(id, check_state()).first;

		// --- Fondo del ítem (se mantiene) ---
		it_anim->second.shader_alpha = ImLerp(
			it_anim->second.shader_alpha, *v ? shader_alpha : 0.f, GetAnimSpeed()
		);
		shaderrt::Draw(
			ImGui::GetWindowDrawList(),
			total_bb.Min, total_bb.Max,
			c::elements::rounding, it_anim->second.shader_alpha, ImShaderTex_WindowBg
		);

		// Opcional: efecto volumétrico del row (si no lo quieres, comenta esta línea)
		draw_volumetric_rect(total_bb.Min, total_bb.Max);

		// --- Color/animación del label (se mantiene) ---
		it_anim->second.label_col = ImLerp(
			it_anim->second.label_col,
			// Usa tus colores preferidos; aquí con "accent" como tenías
			*v ? c::accent : (hovered ? c::accent : c::accent),
			GetAnimSpeed()
		);

		// Toggle lógico (si NO quieres interacción, comenta este bloque)
		if (IsItemClicked()) {
			*v = !(*v);
			MarkItemEdited(id);
		}

		// --- SIN CHECKBOX ---
		// Eliminado: inactive_check_bb, ShadowRect, AddRect (stroke/fill) y AddCircleFilled

		// Solo dibuja el texto animado, centrado verticalmente
		draw_animated_text(
			ImVec2(pos.x + 10.5f, utils::center_text(total_bb.Min, total_bb.Max, label).y),
			id, *v, hovered, label
		);

		return pressed;
	}


	bool CheckboxClicked(const char* label, bool* v)
	{
		ImGuiWindow* window = GetCurrentWindow();

		if (window->SkipItems) return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);
		const ImVec2 label_size = CalcTextSize(label, NULL, true), pos = window->DC.CursorPos;
		const ImRect bb(pos + ImVec2(GetContentRegionMax().x - 70, 0), pos + ImVec2(GetContentRegionMax().x - 50, 32));

		if (!ItemAdd(bb, id)) return false;

		bool hovered, held, pressed = ButtonBehavior(bb, id, &hovered, &held, NULL);

		custom::Checkbox(label, v);

		PushFont(font::icomoon_page);
		GetWindowDrawList()->AddText(pos + ImVec2(GetContentRegionMax().x - 65, 7), GetColorU32(c::text::text), "l");
		PopFont();

		return pressed;
	}


	static float CalcMaxPopupHeightFromItemCount(int items_count)
	{
		ImGuiContext& g = *GImGui;
		if (items_count <= 0)
			return FLT_MAX;
		return (g.FontSize + g.Style.ItemSpacing.y) * items_count - g.Style.ItemSpacing.y + (g.Style.WindowPadding.y * 2);
	}


	struct begin_state
	{
		ImVec4 background, text, outline, highlighting;
		float open, alpha, combo_size = 0.f, shadow_opticaly;
		bool opened_combo = false, hovered = false;
		float arrow_roll, shader_alpha;
	};

	bool BeginCombo(const char* label, const char* preview_value, int val, bool multi, ImGuiComboFlags flags)
	{

		ImGuiContext& g = *GImGui;
		ImGuiWindow* window = GetCurrentWindow();

		g.NextWindowData.ClearFlags();
		if (window->SkipItems) return false;

		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);


		const ImVec2 pos = window->DC.CursorPos;

		const ImVec2 label_size = CalcTextSize(label, NULL, true);
		const float w = ((GetContentRegionMax().x - style.WindowPadding.x));

		const char* display_end = FindRenderedTextEnd(label);

		const ImRect total_bb(pos, pos + ImVec2(w, 47));

		const ImRect bb_box((display_end == label) ? total_bb.Min : (total_bb.Max - ImVec2(total_bb.GetWidth() / 2, total_bb.GetHeight())), total_bb.Max);

		const ImRect bb(bb_box.Min + ImVec2(10.5f, 8.5f), bb_box.Max - ImVec2(10.5f, 8.5f));

		ItemSize(total_bb, 0.f);

		if (!ItemAdd(total_bb, id, &bb)) return false;

		bool hovered, held, pressed = ButtonBehavior(total_bb, id, &hovered, &held);

		static std::map<ImGuiID, begin_state> anim;
		auto it_anim = anim.find(id);

		if (it_anim == anim.end())
		{
			anim.insert({ id, begin_state() });
			it_anim = anim.find(id);
		}

		if (hovered && g.IO.MouseClicked[0] && !combo_popup.is_open()) {	
			it_anim->second.opened_combo = true;
		}

		if (it_anim->second.opened_combo && !it_anim->second.hovered && !hovered && g.IO.MouseClicked[0]) {
			combo_popup.close();
			it_anim->second.opened_combo = false;
		}

		
		it_anim->second.text = ImLerp(it_anim->second.text, it_anim->second.opened_combo ? c::text::label::active : hovered ? c::text::label::active : c::text::label::regular, GetAnimSpeed());
		it_anim->second.combo_size = ImLerp(it_anim->second.combo_size, it_anim->second.opened_combo ? (val * 32) + 23 : 0.f, g.IO.DeltaTime * 12.f);
		it_anim->second.highlighting = ImLerp(it_anim->second.highlighting, hovered ? ImColor(1.f, 1.f, 1.f, 0.1f) : ImColor(1.f, 1.f, 1.f, 0.f), GetAnimSpeed() * 2);
		it_anim->second.shader_alpha = ImLerp(it_anim->second.shader_alpha, it_anim->second.opened_combo ? shader_alpha : 0.f, GetAnimSpeed());

		shaderrt::Draw(ImGui::GetWindowDrawList(), total_bb.Min, total_bb.Max, c::elements::rounding, it_anim->second.shader_alpha, ImShaderTex_WindowBg);


		draw_volumetric_rect(total_bb.Min, total_bb.Max);

		

		if (display_end > label) {
			GetWindowDrawList()->AddText(ImVec2(total_bb.Min.x + 10.5f, utils::center_text(total_bb.Min, total_bb.Max, label).y), GetColorU32(it_anim->second.text), label, display_end);
		}

		if (preview_value && *preview_value) {
			ImVec2 text_pos = utils::center_text(total_bb.Min, total_bb.Max, preview_value);

			// Проверяем, что координаты текста находятся в допустимых пределах
			text_pos.y = ImClamp(text_pos.y, bb.Min.y, bb.Max.y);
			text_pos.x = ImClamp(text_pos.x, bb.Min.x, bb.Max.x);

			PushClipRect(bb.Min, bb.Max, true);

			float drawX = (display_end == label)
				? (total_bb.Min.x + (total_bb.GetWidth() - CalcTextSize(preview_value).x) * 0.5f)
				: (bb.Max.x - 25.f - CalcTextSize(preview_value).x);

			GetWindowDrawList()->AddText(ImVec2(drawX, text_pos.y), c::label::regular, preview_value);
			
			PopClipRect();
		}

		it_anim->second.arrow_roll = ImLerp(it_anim->second.arrow_roll, it_anim->second.opened_combo ? -1.f : 1.f, g.IO.DeltaTime * 6.f);
		ImRotateStart();
		GetWindowDrawList()->AddText(ImVec2(bb.Max.x - (26 + CalcTextSize(ICON_RIGHT_LINE).y) / 2, bb.GetCenter().y - CalcTextSize(ICON_RIGHT_LINE).y / 2), GetColorU32(c::text::text), ICON_RIGHT_LINE);
		ImRotateEnd(1.57f * it_anim->second.arrow_roll);


		if (pressed)
		{
			if (combo_popup.is_open() && strcmp(current_popup_name, label) == 0) {
				combo_popup.close();
				it_anim->second.opened_combo = false;
			}
			else if (!combo_popup.is_open()) {
				combo_popup.open(label);
			}
		}

		if (!IsRectVisible(bb.Min, bb.Max + ImVec2(0, 2)))
		{
			it_anim->second.opened_combo = false;
			it_anim->second.combo_size = 0.f;
		}

		if (!combo_popup.is_open()) return false;

		ImGui::GetIO().WantCaptureMouse = true;

		ImGui::SetNextWindowPos(ImVec2(bb.Max.x - 180, bb.Max.y + 5));
		ImGui::SetNextWindowSize(ImVec2(bb.GetWidth(), it_anim->second.combo_size));

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoScrollWithMouse;

		std::string label_str = label;

		bool ret = combo_popup.begin(200.f, label);

		it_anim->second.hovered = ImGui::GetCurrentWindow()->Rect().Contains(ImGui::GetMousePos());

		if (multi && it_anim->second.hovered && g.IO.MouseClicked[0]) it_anim->second.opened_combo = false;

		return ret;
	}

	void EndCombo()
	{
		combo_popup.end();
	}

	struct menucombo_state
	{
		float content_size, current_scroll, target_scroll, max_scroll;
		float button_width_scale[2];
	};

	void MultiCombo(const char* label, bool variable[], const char* labels[], int count)
	{
		ImGuiContext& g = *GImGui;

		std::string preview = "None";

		for (auto i = 0, j = 0; i < count; i++)
		{
			if (variable[i])
			{
				if (j)
					preview += (", ") + (std::string)labels[i];
				else
					preview = labels[i];

				j++;
			}
		}

		float scroll_step = 150.f;

		if (custom::BeginCombo(label, preview.c_str(), count))
		{
			static std::map<ImGuiID, menucombo_state> anim;
			auto it_anim = anim.emplace(ImGui::GetID(label), menucombo_state()).first;
					
			if (combo_popup.animated_button(ICON_CLOSE_LINE, ImDrawFlags_RoundCornersLeft)) {
				combo_popup.close();
			}

			it_anim->second.button_width_scale[0] = ImLerp(it_anim->second.button_width_scale[0], it_anim->second.current_scroll > 5.f ? 1.f : 0.01f, ImGui::GetIO().DeltaTime * 8.f);
			it_anim->second.button_width_scale[1] = ImLerp(it_anim->second.button_width_scale[1], it_anim->second.current_scroll < it_anim->second.max_scroll - 10.f ? 1.f : 0.01f, ImGui::GetIO().DeltaTime * 8.f);

			if (it_anim->second.button_width_scale[0] > 0.05f) {
				if (combo_popup.animated_button(ICON_LEFT_LINE, (ImDrawFlags)ImDrawFlags_RoundCornersLeft, (float)it_anim->second.button_width_scale[0])) {
					if (it_anim->second.target_scroll > scroll_step)
						it_anim->second.target_scroll -= it_anim->second.target_scroll < 2.f ? scroll_step + 20 : scroll_step;
					else
						it_anim->second.target_scroll = 0.f;
				}
			}


			float default_rounding = c::child::rounding;
			
			ImGui::BeginChild("selectables", ImVec2(ImClamp(ImGui::GetCurrentWindow()->ContentSize.x - (38 * it_anim->second.button_width_scale[1]) - (38 * it_anim->second.button_width_scale[0]), 5.f, 300.f - (38 * it_anim->second.button_width_scale[1]) - (38 * it_anim->second.button_width_scale[0])), ImGui::GetCurrentWindow()->Size.y), false);
			{
				c::child::rounding = 0;
				PushFont(font::bold_small);
				for (auto i = 0; i < count; i++)
				{
					ImDrawFlags flags = i != count - 1 ? ImDrawFlags_RoundCornersAll : ImDrawFlags_RoundCornersRight;
					if (combo_popup.animated_button(labels[i], variable[i], i == count - 1 ? ImDrawFlags_RoundCornersRight : ImDrawFlags_RoundCornersLeft))
						variable[i] = !variable[i];


					it_anim->second.content_size = ImGui::GetCurrentWindow()->ContentSize.x - (38.f * it_anim->second.button_width_scale[0]);

					ImGui::GetCurrentWindow()->Scroll.x = it_anim->second.current_scroll;

					it_anim->second.max_scroll = it_anim->second.content_size - ImGui::GetWindowSize().x - 30.f;
				}
				PopFont();		
			}
			ImGui::EndChild(false);
			
			if (it_anim->second.content_size > 300.f)
			{				
				std::string label_str = label;

				it_anim->second.current_scroll = ImLerp(it_anim->second.current_scroll, it_anim->second.target_scroll, ImGui::GetIO().DeltaTime * 6.f);

				if (it_anim->second.button_width_scale[1] > 0.05f) {
					if (combo_popup.animated_button(ICON_RIGHT_LINE, ImDrawFlags_RoundCornersRight)) {
						
							if (it_anim->second.target_scroll + scroll_step < it_anim->second.max_scroll)
								it_anim->second.target_scroll += scroll_step;
							else
								it_anim->second.target_scroll = it_anim->second.max_scroll;
					}
				}
			}

			custom::EndCombo();

			c::child::rounding = default_rounding;
		}

		preview = ("None");
	}

	bool BeginComboPreview()
	{
		ImGuiContext& g = *GImGui;
		ImGuiWindow* window = g.CurrentWindow;
		ImGuiComboPreviewData* preview_data = &g.ComboPreviewData;

		if (window->SkipItems || !(g.LastItemData.StatusFlags & ImGuiItemStatusFlags_Visible)) return false;

		IM_ASSERT(g.LastItemData.Rect.Min.x == preview_data->PreviewRect.Min.x && g.LastItemData.Rect.Min.y == preview_data->PreviewRect.Min.y);

		if (!window->ClipRect.Overlaps(preview_data->PreviewRect)) return false;

		preview_data->BackupCursorPos = window->DC.CursorPos;
		preview_data->BackupCursorMaxPos = window->DC.CursorMaxPos;
		preview_data->BackupCursorPosPrevLine = window->DC.CursorPosPrevLine;
		preview_data->BackupPrevLineTextBaseOffset = window->DC.PrevLineTextBaseOffset;
		preview_data->BackupLayout = window->DC.LayoutType;
		window->DC.CursorPos = preview_data->PreviewRect.Min + g.Style.FramePadding;
		window->DC.CursorMaxPos = window->DC.CursorPos;
		window->DC.LayoutType = ImGuiLayoutType_Horizontal;
		window->DC.IsSameLine = false;
		PushClipRect(preview_data->PreviewRect.Min, preview_data->PreviewRect.Max, true);

		return true;
	}

	void EndComboPreview()
	{
		ImGuiContext& g = *GImGui;
		ImGuiWindow* window = g.CurrentWindow;
		ImGuiComboPreviewData* preview_data = &g.ComboPreviewData;

		ImDrawList* draw_list = window->DrawList;
		if (window->DC.CursorMaxPos.x < preview_data->PreviewRect.Max.x && window->DC.CursorMaxPos.y < preview_data->PreviewRect.Max.y)
			if (draw_list->CmdBuffer.Size > 1)
			{
				draw_list->_CmdHeader.ClipRect = draw_list->CmdBuffer[draw_list->CmdBuffer.Size - 1].ClipRect = draw_list->CmdBuffer[draw_list->CmdBuffer.Size - 2].ClipRect;
				draw_list->_TryMergeDrawCmds();
			}
		PopClipRect();
		window->DC.CursorPos = preview_data->BackupCursorPos;
		window->DC.CursorMaxPos = ImMax(window->DC.CursorMaxPos, preview_data->BackupCursorMaxPos);
		window->DC.CursorPosPrevLine = preview_data->BackupCursorPosPrevLine;
		window->DC.PrevLineTextBaseOffset = preview_data->BackupPrevLineTextBaseOffset;
		window->DC.LayoutType = preview_data->BackupLayout;
		window->DC.IsSameLine = false;
		preview_data->PreviewRect = ImRect();
	}

	static const char* Items_ArrayGetter(void* data, int idx)
	{
		const char* const* items = (const char* const*)data;
		return items[idx];
	}

	static const char* Items_SingleStringGetter(void* data, int idx)
	{
		const char* items_separated_by_zeros = (const char*)data;
		int items_count = 0;
		const char* p = items_separated_by_zeros;
		while (*p)
		{
			if (idx == items_count)
				break;
			p += strlen(p) + 1;
			items_count++;
		}
		return *p ? p : NULL;
	}


	bool Combo(const char* label, int* current_item, const char* (*getter)(void* user_data, int idx), void* user_data, int items_count, int popup_max_height_in_items)
	{
		ImGuiContext& g = *GImGui;

		const char* preview_value = (*current_item >= 0 && *current_item < items_count) ? getter(user_data, *current_item) : "None";

		if (popup_max_height_in_items != -1 && !(g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSizeConstraint))
			SetNextWindowSizeConstraints(ImVec2(0, 0), ImVec2(FLT_MAX, CalcMaxPopupHeightFromItemCount(popup_max_height_in_items)));

		if (!custom::BeginCombo(label, preview_value, items_count)) return false;

		static std::map<ImGuiID, menucombo_state> anim;
		auto it_anim = anim.emplace(ImGui::GetID(label), menucombo_state()).first;

		bool value_changed = false;

		// Анимация кнопок прокрутки
		it_anim->second.button_width_scale[0] = ImLerp(it_anim->second.button_width_scale[0], it_anim->second.current_scroll > 5.f ? 1.f : 0.01f, ImGui::GetIO().DeltaTime * 8.f);
		it_anim->second.button_width_scale[1] = ImLerp(it_anim->second.button_width_scale[1], it_anim->second.current_scroll < it_anim->second.max_scroll - 10.f ? 1.f : 0.01f, ImGui::GetIO().DeltaTime * 8.f);

		// Кнопка прокрутки влево
		if (it_anim->second.button_width_scale[0] > 0.05f)
		{
			if (combo_popup.animated_button(ICON_LEFT_LINE, (ImDrawFlags)ImDrawFlags_RoundCornersLeft, (float)it_anim->second.button_width_scale[0]))
			{
				if (it_anim->second.target_scroll > 150.f)
					it_anim->second.target_scroll -= it_anim->second.target_scroll < 2.f ? 170.f : 150.f;
				else
					it_anim->second.target_scroll = 0.f;
			}
			
		}

		float default_rounding = c::child::rounding;

			c::child::rounding = 0;
			PushFont(font::bold_small);

			for (int i = 0; i < items_count; i++)
			{
				const char* item_text = getter(user_data, i);
				if (!item_text)
					item_text = "*Unknown item*";

				// Выбор одного элемента
				if(custom::Selectable(item_text, i == *current_item, 0, ImVec2(ImGui::GetContentRegionAvail().x, 35))) {
					if (*current_item != i)
					{
						*current_item = i;
						value_changed = true;
					}
				}
				
				it_anim->second.content_size = ImGui::GetCurrentWindow()->ContentSize.x - (38.f * it_anim->second.button_width_scale[0]);
			}

			PopFont();
		
		// Кнопка прокрутки вправо
		if (it_anim->second.content_size > 300.f)
		{
			it_anim->second.current_scroll = ImLerp(it_anim->second.current_scroll, it_anim->second.target_scroll, ImGui::GetIO().DeltaTime * 6.f);

			if (it_anim->second.button_width_scale[1] > 0.05f)
			{
				if (combo_popup.animated_button(ICON_RIGHT_LINE, ImDrawFlags_RoundCornersRight))
				{
					if (it_anim->second.target_scroll + 150.f < it_anim->second.max_scroll)
						it_anim->second.target_scroll += 150.f;
					else
						it_anim->second.target_scroll = it_anim->second.max_scroll;
				}
			}
		}

		custom::EndCombo();
		c::child::rounding = default_rounding;

		if (value_changed)
			MarkItemEdited(g.LastItemData.ID);

		return value_changed;
	}

	bool Combo(const char* label, int* current_item, const char* const items[], int items_count, int height_in_items)
	{
		const bool value_changed = Combo(label, current_item, Items_ArrayGetter, (void*)items, items_count, height_in_items);
		return value_changed;
	}

	bool Combo(const char* label, int* current_item, const char* items_separated_by_zeros, int height_in_items)
	{
		int items_count = 0;
		const char* p = items_separated_by_zeros;
		while (*p)
		{
			p += strlen(p) + 1;
			items_count++;
		}
		bool value_changed = Combo(label, current_item, Items_SingleStringGetter, (void*)items_separated_by_zeros, items_count, height_in_items);
		return value_changed;
	}



	struct select_state
	{
		ImVec4 text, background, stroke;
		float circle_radius, text_offset;
	};

	bool Selectable(const char* label, bool selected, ImGuiSelectableFlags flags, const ImVec2& size_arg)
	{
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;

		ImGuiID id = window->GetID(label);
		ImVec2 label_size = CalcTextSize(label, NULL, true);
		ImVec2 size(size_arg.x != 0.0f ? size_arg.x : label_size.x, size_arg.y != 0.0f ? size_arg.y : label_size.y);
		ImVec2 pos = window->DC.CursorPos;
		pos.y += window->DC.CurrLineTextBaseOffset;
		ItemSize(size, 0.0f);

		const bool span_all_columns = (flags & ImGuiSelectableFlags_SpanAllColumns) != 0;
		const float min_x = span_all_columns ? window->ParentWorkRect.Min.x : pos.x;
		const float max_x = span_all_columns ? window->ParentWorkRect.Max.x : window->WorkRect.Max.x;
		if (size_arg.x == 0.0f || (flags & ImGuiSelectableFlags_SpanAvailWidth)) size.x = ImMax(label_size.x, max_x - min_x);

		const ImVec2 text_min = pos;
		const ImVec2 text_max(min_x + size.x, pos.y + size.y);

		ImRect bb(min_x, pos.y, text_max.x, text_max.y);
		if ((flags & ImGuiSelectableFlags_NoPadWithHalfSpacing) == 0)
		{
			const float spacing_x = span_all_columns ? 0.0f : style.ItemSpacing.x;
			const float spacing_y = style.ItemSpacing.y;
			const float spacing_L = IM_TRUNC(spacing_x * 0.50f);
			const float spacing_U = IM_TRUNC(spacing_y * 0.50f);
			bb.Min.x -= spacing_L;
			bb.Min.y -= spacing_U;
			bb.Max.x += (spacing_x - spacing_L);
			bb.Max.y += (spacing_y - spacing_U);
		}

		const float backup_clip_rect_min_x = window->ClipRect.Min.x;
		const float backup_clip_rect_max_x = window->ClipRect.Max.x;
		if (span_all_columns)
		{
			window->ClipRect.Min.x = window->ParentWorkRect.Min.x;
			window->ClipRect.Max.x = window->ParentWorkRect.Max.x;
		}

		const bool disabled_item = (flags & ImGuiSelectableFlags_Disabled) != 0;
		const bool item_add = ItemAdd(bb, id, NULL, disabled_item ? ImGuiItemFlags_Disabled : ImGuiItemFlags_None);
		if (span_all_columns)
		{
			window->ClipRect.Min.x = backup_clip_rect_min_x;
			window->ClipRect.Max.x = backup_clip_rect_max_x;
		}

		if (!item_add) return false;

		const bool disabled_global = (g.CurrentItemFlags & ImGuiItemFlags_Disabled) != 0;
		if (disabled_item && !disabled_global) BeginDisabled();

		if (span_all_columns && window->DC.CurrentColumns) PushColumnsBackground();
		else if (span_all_columns && g.CurrentTable) TablePushBackgroundChannel();

		ImGuiButtonFlags button_flags = 0;
		if (flags & ImGuiSelectableFlags_NoHoldingActiveID) { button_flags |= ImGuiButtonFlags_NoHoldingActiveId; }
		if (flags & ImGuiSelectableFlags_NoSetKeyOwner) { button_flags |= ImGuiButtonFlags_NoSetKeyOwner; }
		if (flags & ImGuiSelectableFlags_SelectOnClick) { button_flags |= ImGuiButtonFlags_PressedOnClick; }
		if (flags & ImGuiSelectableFlags_SelectOnRelease) { button_flags |= ImGuiButtonFlags_PressedOnRelease; }
		if (flags & ImGuiSelectableFlags_AllowDoubleClick) { button_flags |= ImGuiButtonFlags_PressedOnClickRelease | ImGuiButtonFlags_PressedOnDoubleClick; }
		if ((flags & ImGuiSelectableFlags_AllowOverlap) || (g.LastItemData.InFlags & ImGuiItemFlags_AllowOverlap)) { button_flags |= ImGuiButtonFlags_AllowOverlap; }

		const bool was_selected = selected;
		bool hovered, held, pressed = ButtonBehavior(bb, id, &hovered, &held, button_flags);

		if ((flags & ImGuiSelectableFlags_SelectOnNav) && g.NavJustMovedToId != 0 && g.NavJustMovedToFocusScopeId == g.CurrentFocusScopeId)
			if (g.NavJustMovedToId == id)  selected = pressed = true;

		// Update NavId when clicking or when Hovering (this doesn't happen on most widgets), so navigation can be resumed with gamepad/keyboard
		if (pressed || (hovered && (flags & ImGuiSelectableFlags_SetNavIdOnHover)))
		{
			if (!g.NavDisableMouseHover && g.NavWindow == window && g.NavLayer == window->DC.NavLayerCurrent)
			{
				SetNavID(id, window->DC.NavLayerCurrent, g.CurrentFocusScopeId, WindowRectAbsToRel(window, bb)); // (bb == NavRect)
				g.NavDisableHighlight = true;
			}
		}
		if (pressed) MarkItemEdited(id);

		if (selected != was_selected)  g.LastItemData.StatusFlags |= ImGuiItemStatusFlags_ToggledSelection;


		if (g.NavId == id) RenderNavHighlight(bb, id, ImGuiNavHighlightFlags_TypeThin | ImGuiNavHighlightFlags_NoRounding);

		if (span_all_columns && window->DC.CurrentColumns) PopColumnsBackground();
		else if (span_all_columns && g.CurrentTable) TablePopBackgroundChannel();

		static std::map<ImGuiID, select_state> anim;
		auto it_anim = anim.find(id);

		if (it_anim == anim.end())
		{
			anim.insert({ id, select_state() });
			it_anim = anim.find(id);
		}

		it_anim->second.text = ImLerp(it_anim->second.text, selected ? c::label::active : c::label::regular, GetAnimSpeed());
		it_anim->second.circle_radius = ImLerp(it_anim->second.circle_radius, selected ? 3.f : 0.f, GetAnimSpeed());
		it_anim->second.text_offset = ImLerp(it_anim->second.text_offset, selected ? 20.f : 4.5f, GetAnimSpeed());
		it_anim->second.background = ImLerp(it_anim->second.background, selected ? c::second_color : utils::GetColorWithAlpha(c::second_color, 0.f), GetAnimSpeed());
		it_anim->second.stroke = ImLerp(it_anim->second.stroke, selected ? utils::GetColorWithAlpha(c::stroke_color, c::stroke_color.Value.w) : utils::GetColorWithAlpha(c::stroke_color, 0.f), GetAnimSpeed());

		window->DrawList->AddRectFilled(bb.Min, bb.Max, GetColorU32(it_anim->second.background), c::elements::rounding);
		window->DrawList->AddRect(bb.Min, bb.Max, GetColorU32(it_anim->second.stroke), c::elements::rounding);

		window->DrawList->AddCircleFilled(ImVec2(bb.Min.x + 9, bb.GetCenter().y), it_anim->second.circle_radius, c::main_color);

		PushStyleColor(ImGuiCol_Text, GetColorU32(it_anim->second.text));
		window->DrawList->AddText(ImVec2(bb.Min.x + it_anim->second.text_offset, utils::center_text(bb.Min, bb.Max, label).y), GetColorU32(it_anim->second.text), label);
		PopStyleColor();

		if (pressed && (window->Flags & ImGuiWindowFlags_Popup) && !(flags & ImGuiSelectableFlags_DontClosePopups) && !(g.LastItemData.InFlags & ImGuiItemFlags_SelectableDontClosePopup)) CloseCurrentPopup();

		if (disabled_item && !disabled_global) EndDisabled();

		return pressed;
	}

	bool Selectable(const char* label, bool* p_selected, ImGuiSelectableFlags flags, const ImVec2& size_arg)
	{
		if (Selectable(label, *p_selected, flags, size_arg))
		{
			*p_selected = !*p_selected;
			return true;
		}
		return false;
	}

	static void ColorEditRestoreH(const float* col, float* H)
	{
		ImGuiContext& g = *GImGui;
		IM_ASSERT(g.ColorEditCurrentID != 0);
		if (g.ColorEditSavedID != g.ColorEditCurrentID || g.ColorEditSavedColor != ImGui::ColorConvertFloat4ToU32(ImVec4(col[0], col[1], col[2], 0)))
			return;
		*H = g.ColorEditSavedHue;
	}

	static void ColorEditRestoreHS(const float* col, float* H, float* S, float* V)
	{
		ImGuiContext& g = *GImGui;
		IM_ASSERT(g.ColorEditCurrentID != 0);
		if (g.ColorEditSavedID != g.ColorEditCurrentID || g.ColorEditSavedColor != ImGui::ColorConvertFloat4ToU32(ImVec4(col[0], col[1], col[2], 0))) return;

		if (*S == 0.0f || (*H == 0.0f && g.ColorEditSavedHue == 1))
			*H = g.ColorEditSavedHue;

		if (*V == 0.0f) *S = g.ColorEditSavedSat;
	}


	struct edit_state
	{
		ImVec4 text;
		ImVec4 icon;
		ImVec4 text_color, description_col;	
		float alpha, shader_alpha;
	};

	bool ColorEdit4(const char* label, const char* description, float col[4], ImGuiColorEditFlags flags)
	{
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems) return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const float square_sz = GetFrameHeight();
		const float w_full = CalcItemWidth();
		const float w_button = (flags & ImGuiColorEditFlags_NoSmallPreview) ? 0.0f : (square_sz + style.ItemInnerSpacing.x);
		const float w_inputs = w_full - w_button;
		const char* label_display_end = FindRenderedTextEnd(label);
		g.NextItemData.ClearFlags();

		BeginGroup();
		PushID(label);
		const bool set_current_color_edit_id = (g.ColorEditCurrentID == 0);
		if (set_current_color_edit_id)
			g.ColorEditCurrentID = window->IDStack.back();

		// If we're not showing any slider there's no point in doing any HSV conversions
		const ImGuiColorEditFlags flags_untouched = flags;
		if (flags & ImGuiColorEditFlags_NoInputs)
			flags = (flags & (~ImGuiColorEditFlags_DisplayMask_)) | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_NoOptions;

		// Context menu: display and modify options (before defaults are applied)
		if (!(flags & ImGuiColorEditFlags_NoOptions))
			ColorEditOptionsPopup(col, flags);

		// Read stored options
		if (!(flags & ImGuiColorEditFlags_DisplayMask_))
			flags |= (g.ColorEditOptions & ImGuiColorEditFlags_DisplayMask_);
		if (!(flags & ImGuiColorEditFlags_DataTypeMask_))
			flags |= (g.ColorEditOptions & ImGuiColorEditFlags_DataTypeMask_);
		if (!(flags & ImGuiColorEditFlags_PickerMask_))
			flags |= (g.ColorEditOptions & ImGuiColorEditFlags_PickerMask_);
		if (!(flags & ImGuiColorEditFlags_InputMask_))
			flags |= (g.ColorEditOptions & ImGuiColorEditFlags_InputMask_);
		flags |= (g.ColorEditOptions & ~(ImGuiColorEditFlags_DisplayMask_ | ImGuiColorEditFlags_DataTypeMask_ | ImGuiColorEditFlags_PickerMask_ | ImGuiColorEditFlags_InputMask_));
		IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiColorEditFlags_DisplayMask_)); // Check that only 1 is selected
		IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiColorEditFlags_InputMask_));   // Check that only 1 is selected

		const bool alpha = (flags & ImGuiColorEditFlags_NoAlpha) == 0;
		const bool hdr = (flags & ImGuiColorEditFlags_HDR) != 0;
		const int components = alpha ? 4 : 3;

		// Convert to the formats we need
		float f[4] = { col[0], col[1], col[2], alpha ? col[3] : 1.0f };
		if ((flags & ImGuiColorEditFlags_InputHSV) && (flags & ImGuiColorEditFlags_DisplayRGB))
			ColorConvertHSVtoRGB(f[0], f[1], f[2], f[0], f[1], f[2]);
		else if ((flags & ImGuiColorEditFlags_InputRGB) && (flags & ImGuiColorEditFlags_DisplayHSV))
		{
			// Hue is lost when converting from grayscale rgb (saturation=0). Restore it.
			ColorConvertRGBtoHSV(f[0], f[1], f[2], f[0], f[1], f[2]);
			ColorEditRestoreHS(col, &f[0], &f[1], &f[2]);
		}
		int i[4] = { IM_F32_TO_INT8_UNBOUND(f[0]), IM_F32_TO_INT8_UNBOUND(f[1]), IM_F32_TO_INT8_UNBOUND(f[2]), IM_F32_TO_INT8_UNBOUND(f[3]) };

		bool value_changed = false;
		bool value_changed_as_float = false;

		const ImVec2 pos = window->DC.CursorPos;
		const float inputs_offset_x = (style.ColorButtonPosition == ImGuiDir_Left) ? w_button : 0.0f;
		window->DC.CursorPos.x = pos.x + inputs_offset_x;

		if ((flags & (ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_DisplayHSV)) != 0 && (flags & ImGuiColorEditFlags_NoInputs) == 0)
		{
			// RGB/HSV 0..255 Sliders
			const float w_item_one = ImMax(1.0f, IM_FLOOR((w_inputs - (style.ItemInnerSpacing.x) * (components - 1)) / (float)components));
			const float w_item_last = ImMax(1.0f, IM_FLOOR(w_inputs - (w_item_one + style.ItemInnerSpacing.x) * (components - 1)));

			const bool hide_prefix = (w_item_one <= CalcTextSize((flags & ImGuiColorEditFlags_Float) ? "M:0.000" : "M:000").x);
			static const char* ids[4] = { "##X", "##Y", "##Z", "##W" };
			static const char* fmt_table_int[3][4] =
			{
				{   "%3d",   "%3d",   "%3d",   "%3d" }, // Short display
				{ "R:%3d", "G:%3d", "B:%3d", "A:%3d" }, // Long display for RGBA
				{ "H:%3d", "S:%3d", "V:%3d", "A:%3d" }  // Long display for HSVA
			};
			static const char* fmt_table_float[3][4] =
			{
				{   "%0.3f",   "%0.3f",   "%0.3f",   "%0.3f" }, // Short display
				{ "R:%0.3f", "G:%0.3f", "B:%0.3f", "A:%0.3f" }, // Long display for RGBA
				{ "H:%0.3f", "S:%0.3f", "V:%0.3f", "A:%0.3f" }  // Long display for HSVA
			};
			const int fmt_idx = hide_prefix ? 0 : (flags & ImGuiColorEditFlags_DisplayHSV) ? 2 : 1;

			for (int n = 0; n < components; n++)
			{
				if (n > 0)
					SameLine(0, style.ItemInnerSpacing.x);
				SetNextItemWidth((n + 1 < components) ? w_item_one : w_item_last);

				// FIXME: When ImGuiColorEditFlags_HDR flag is passed HS values snap in weird ways when SV values go below 0.
				if (flags & ImGuiColorEditFlags_Float)
				{
					value_changed |= DragFloat(ids[n], &f[n], 1.0f / 255.0f, 0.0f, hdr ? 0.0f : 1.0f, fmt_table_float[fmt_idx][n]);
					value_changed_as_float |= value_changed;
				}
				else
				{
					value_changed |= DragInt(ids[n], &i[n], 1.0f, 0, hdr ? 0 : 255, fmt_table_int[fmt_idx][n]);
				}
				if (!(flags & ImGuiColorEditFlags_NoOptions))
					OpenPopupOnItemClick("context", ImGuiPopupFlags_MouseButtonRight);
			}
		}
		else if ((flags & ImGuiColorEditFlags_DisplayHex) != 0 && (flags & ImGuiColorEditFlags_NoInputs) == 0)
		{

			if (!(flags & ImGuiColorEditFlags_NoOptions))
				OpenPopupOnItemClick("context", ImGuiPopupFlags_MouseButtonRight);
		}

		// RGB Hexadecimal Input
		char buf[64];
		ImFormatString(buf, IM_ARRAYSIZE(buf), "#%02X%02X%02X", ImClamp(i[0], 0, 255), ImClamp(i[1], 0, 255), ImClamp(i[2], 0, 255));

		const float width = GetContentRegionAvail().x;


		const ImRect rect(window->DC.CursorPos, window->DC.CursorPos + ImVec2(width, 35));

		const ImVec4 col_v4(col[0], col[1], col[2], alpha ? col[3] : 1.0f);


		static std::map<ImGuiID, edit_state> anim;
		auto it_anim = anim.find(ImGui::GetID(label));

		if (it_anim == anim.end())
		{
			anim.insert({ ImGui::GetID(label), edit_state() });
			it_anim = anim.find(ImGui::GetID(label));
		}

		ImGuiWindow* picker_active_window = NULL;
		if (!(flags & ImGuiColorEditFlags_NoSmallPreview))
		{
			const float button_offset_x = ((flags & ImGuiColorEditFlags_NoInputs) || (style.ColorButtonPosition == ImGuiDir_Left)) ? 0.0f : w_inputs + style.ItemInnerSpacing.x;
			window->DC.CursorPos = ImVec2(pos.x, pos.y);

			if (IsMouseHoveringRect(rect.Min, rect.Max, true) && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{
				if (!(flags & ImGuiColorEditFlags_NoPicker))
				{
					g.ColorPickerRef = col_v4;
					OpenPopup("picker");
					SetNextWindowPos(ImGui::GetMousePos() - ImVec2(115, 45));
				}
			}
			if (!(flags & ImGuiColorEditFlags_NoOptions))
				OpenPopupOnItemClick("context", ImGuiPopupFlags_MouseButtonRight);

			PushStyleVar(ImGuiStyleVar_PopupRounding, c::child::rounding);
			PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(14, 14));
			PushStyleVar(ImGuiStyleVar_Alpha, it_anim->second.alpha);

			static bool active_popup;

			if (BeginPopup("picker", ImGuiWindowFlags_NoBackground))
			{
				ImRect bb = ImGui::GetCurrentWindow()->Rect();
								
				ImGui::PushClipRect(ImVec2(0, 0), ImGui::GetMainViewport()->Size, false);
				ImGui::GetWindowDrawList()->AddRectFilled(bb.Min, bb.Max, c::second_color, c::elements::rounding);
				ImGui::GetWindowDrawList()->AddRect(bb.Min, bb.Max, ImColor(1.f, 1.f, 1.f, 0.05f), c::elements::rounding);
				ImGui::PopClipRect();

				if (g.CurrentWindow->BeginCount == 1)
				{
					picker_active_window = g.CurrentWindow;

					ImGuiColorEditFlags picker_flags_to_forward = ImGuiColorEditFlags_DataTypeMask_ | ImGuiColorEditFlags_PickerMask_ | ImGuiColorEditFlags_InputMask_ | ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_AlphaBar;
					ImGuiColorEditFlags picker_flags = (flags_untouched & picker_flags_to_forward) | ImGuiColorEditFlags_DisplayMask_ | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaPreviewHalf;
					SetNextItemWidth(square_sz * 9.5f); // Reduced size from 15.0f to 9.5f
					value_changed |= ColorPicker4("##picker", col, picker_flags, &g.ColorPickerRef.x);
				}
				EndPopup();
			}
			PopStyleVar(3);
		}

		if (label != label_display_end && !(flags & ImGuiColorEditFlags_NoLabel))
		{


			SameLine(0.0f, style.ItemInnerSpacing.x);
			window->DC.CursorPos.x = pos.x - w_button + ((flags & ImGuiColorEditFlags_NoInputs) ? w_button : w_full);

			const ImVec2 check_offset = ImVec2(8.f, 8.f);

			ImRect check_bb(rect.Max - ImVec2(rect.GetSize().y, rect.GetSize().y) + check_offset, rect.Max - check_offset);


			it_anim->second.alpha = ImLerp(it_anim->second.alpha, ImGui::IsPopupOpen("picker") ? 1.f : 0.f, GetAnimSpeed());

			it_anim->second.text = ImLerp(it_anim->second.text, ImGui::IsPopupOpen("picker") ? c::text::label::active : IsMouseHoveringRect(rect.Min, rect.Max, true) ? c::text::label::active : c::text::label::regular, GetAnimSpeed());

			it_anim->second.icon = ImLerp(it_anim->second.icon, ImGui::IsPopupOpen("picker") ? c::main_color : IsMouseHoveringRect(rect.Min, rect.Max, true) ? c::text::label::hovered : c::text::label::regular, GetAnimSpeed());

			it_anim->second.text_color = ImLerp(it_anim->second.text_color, ImGui::IsPopupOpen("picker") ? c::text::label::active : rect.Contains(ImGui::GetMousePos()) ? c::text::label::active : c::text::label::regular, GetAnimSpeed());

			it_anim->second.shader_alpha = ImLerp(it_anim->second.shader_alpha, ImGui::IsPopupOpen("picker") ? 1.f : 0.f, GetAnimSpeed());


			ImColor color_rgb = col_v4;

			
			shaderrt::Draw(ImGui::GetWindowDrawList(), rect.Min, rect.Max, c::elements::rounding, it_anim->second.shader_alpha, ImShaderTex_WindowBg);

			draw_volumetric_rect(rect.Min, rect.Max);

			GetWindowDrawList()->AddRectFilled(check_bb.Min, check_bb.Max, color_rgb, c::elements::rounding);

			RenderColorRectWithAlphaCheckerboard(window->DrawList, check_bb.Min, check_bb.Max, color_rgb, ImMin(36, 29) / 2.99f, ImVec2(0.f, 0.f), c::elements::rounding);

			ImGui::PushClipRect(rect.Min, rect.Max, true);
			GetWindowDrawList()->AddText(ImVec2(rect.Min.x + 10.4f, rect.GetCenter().y - CalcTextSize(label).y / 2), GetColorU32(it_anim->second.text_color), label);
			ImGui::PopClipRect();

			ImGui::SetCursorScreenPos(ImVec2(rect.Min.x, rect.Max.y));
		}

		// Convert back
		if (value_changed && picker_active_window == NULL)
		{
			if (!value_changed_as_float)
				for (int n = 0; n < 4; n++)
					f[n] = i[n] / 255.0f;
			if ((flags & ImGuiColorEditFlags_DisplayHSV) && (flags & ImGuiColorEditFlags_InputRGB))
			{
				g.ColorEditSavedHue = f[0];
				g.ColorEditSavedSat = f[1];
				ColorConvertHSVtoRGB(f[0], f[1], f[2], f[0], f[1], f[2]);
				g.ColorEditSavedID = g.ColorEditCurrentID;
				g.ColorEditSavedColor = ColorConvertFloat4ToU32(ImVec4(f[0], f[1], f[2], 0));
			}
			if ((flags & ImGuiColorEditFlags_DisplayRGB) && (flags & ImGuiColorEditFlags_InputHSV))
				ColorConvertRGBtoHSV(f[0], f[1], f[2], f[0], f[1], f[2]);

			col[0] = f[0];
			col[1] = f[1];
			col[2] = f[2];
			if (alpha)
				col[3] = f[3];
		}

		if (set_current_color_edit_id)
			g.ColorEditCurrentID = 0;
		PopID();
		EndGroup();

		// Drag and Drop Target
		// NB: The flag test is merely an optional micro-optimization, BeginDragDropTarget() does the same test.
		if ((g.LastItemData.StatusFlags & ImGuiItemStatusFlags_HoveredRect) && !(flags & ImGuiColorEditFlags_NoDragDrop) && BeginDragDropTarget())
		{
			bool accepted_drag_drop = false;
			if (const ImGuiPayload* payload = AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_3F))
			{
				memcpy((float*)col, payload->Data, sizeof(float) * 3); // Preserve alpha if any //-V512 //-V1086
				value_changed = accepted_drag_drop = true;
			}
			if (const ImGuiPayload* payload = AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_4F))
			{
				memcpy((float*)col, payload->Data, sizeof(float) * components);
				value_changed = accepted_drag_drop = true;
			}

			// Drag-drop payloads are always RGB
			if (accepted_drag_drop && (flags & ImGuiColorEditFlags_InputHSV))
				ColorConvertRGBtoHSV(col[0], col[1], col[2], col[0], col[1], col[2]);
			EndDragDropTarget();
		}

		// When picker is being actively used, use its active id so IsItemActive() will function on ColorEdit4().
		if (picker_active_window && g.ActiveId != 0 && g.ActiveIdWindow == picker_active_window)
			g.LastItemData.ID = g.ActiveId;

		if (value_changed && g.LastItemData.ID != 0) // In case of ID collision, the second EndGroup() won't catch g.ActiveId
			MarkItemEdited(g.LastItemData.ID);

		return value_changed;
	}


	// Helper for ColorPicker4()
	static void RenderArrowsForVerticalBar(ImDrawList* draw_list, ImVec2 pos, ImVec2 half_sz, float bar_w, float alpha)
	{
		ImU32 alpha8 = IM_F32_TO_INT8_SAT(alpha);
		ImGui::RenderArrowPointingAt(draw_list, ImVec2(pos.x + half_sz.x + 1, pos.y), ImVec2(half_sz.x + 2, half_sz.y + 1), ImGuiDir_Right, IM_COL32(0, 0, 0, alpha8));
		ImGui::RenderArrowPointingAt(draw_list, ImVec2(pos.x + half_sz.x, pos.y), half_sz, ImGuiDir_Right, IM_COL32(255, 255, 255, alpha8));
		ImGui::RenderArrowPointingAt(draw_list, ImVec2(pos.x + bar_w - half_sz.x - 1, pos.y), ImVec2(half_sz.x + 2, half_sz.y + 1), ImGuiDir_Left, IM_COL32(0, 0, 0, alpha8));
		ImGui::RenderArrowPointingAt(draw_list, ImVec2(pos.x + bar_w - half_sz.x, pos.y), half_sz, ImGuiDir_Left, IM_COL32(255, 255, 255, alpha8));
	}

	struct picker_state
	{
		float hue_bar;
		float alpha_bar;
		float circle;
		ImVec2 circle_move;
	};

	bool ColorPicker4(const char* label, float col[4], ImGuiColorEditFlags flags, const float* ref_col)
	{
		ImGuiContext& g = *GImGui;
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImDrawList* draw_list = window->DrawList;
		ImGuiStyle& style = g.Style;
		ImGuiIO& io = g.IO;

		const float width = CalcItemWidth();
		g.NextItemData.ClearFlags();

		PushID(label);
		BeginGroup();

		if (!(flags & ImGuiColorEditFlags_NoSidePreview))
			flags |= ImGuiColorEditFlags_NoSmallPreview;

		if (!(flags & ImGuiColorEditFlags_NoOptions))
			ColorPickerOptionsPopup(col, flags);

		// Read stored options
		if (!(flags & ImGuiColorEditFlags_PickerMask_))
			flags |= ((g.ColorEditOptions & ImGuiColorEditFlags_PickerMask_) ? g.ColorEditOptions : ImGuiColorEditFlags_DefaultOptions_) & ImGuiColorEditFlags_PickerMask_;
		if (!(flags & ImGuiColorEditFlags_InputMask_))
			flags |= ((g.ColorEditOptions & ImGuiColorEditFlags_InputMask_) ? g.ColorEditOptions : ImGuiColorEditFlags_DefaultOptions_) & ImGuiColorEditFlags_InputMask_;
		IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiColorEditFlags_PickerMask_)); // Check that only 1 is selected
		IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiColorEditFlags_InputMask_));  // Check that only 1 is selected
		if (!(flags & ImGuiColorEditFlags_NoOptions))
			flags |= (g.ColorEditOptions & ImGuiColorEditFlags_AlphaBar);

		// Setup
		int components = (flags & ImGuiColorEditFlags_NoAlpha) ? 3 : 4;
		bool alpha_bar = (flags & ImGuiColorEditFlags_AlphaBar) && !(flags & ImGuiColorEditFlags_NoAlpha);
		ImVec2 picker_pos = window->DC.CursorPos;

		float bars_width = 40.f; // Arbitrary smallish width of Hue/Alpha picking bars
		float sv_picker_size = ImMax(bars_width * 1, width - (alpha_bar ? 2 : 1) * (bars_width + style.ItemInnerSpacing.x)) / 2; // Saturation/Value picking box

		ImVec2 bar_pos = window->DC.CursorPos + ImVec2(0, sv_picker_size);
		float square_sz = GetFrameHeight();

		float sv_bar_size = 20; // Saturation/Value picking box
		float bar0_pos_x = GetWindowPos().x + style.WindowPadding.x;
		float bar1_pos_x = bar0_pos_x;
		float bars_triangles_half_sz = IM_FLOOR(bars_width * 0.20f);

		float backup_initial_col[4];
		memcpy(backup_initial_col, col, components * sizeof(float));

		float H = col[0], S = col[1], V = col[2];
		float R = col[0], G = col[1], B = col[2];
		if (flags & ImGuiColorEditFlags_InputRGB)
		{
			// Hue is lost when converting from greyscale rgb (saturation=0). Restore it.
			ColorConvertRGBtoHSV(R, G, B, H, S, V);
			ColorEditRestoreHS(col, &H, &S, &V);
		}
		else if (flags & ImGuiColorEditFlags_InputHSV)
		{
			ColorConvertHSVtoRGB(H, S, V, R, G, B);
		}

		bool value_changed = false, value_changed_h = false, value_changed_sv = false;

		PushItemFlag(ImGuiItemFlags_NoNav, true);

		// SV rectangle logic
		InvisibleButton("sv", ImVec2(sv_picker_size, sv_picker_size));
		if (IsItemActive())
		{
			S = ImSaturate((io.MousePos.x - picker_pos.x) / (sv_picker_size - 1));
			V = 1.0f - ImSaturate((io.MousePos.y - picker_pos.y) / (sv_picker_size));

			// Greatly reduces hue jitter and reset to 0 when hue == 255 and color is rapidly modified using SV square.
			if (g.ColorEditSavedColor == ColorConvertFloat4ToU32(ImVec4(col[0], col[1], col[2], 0)))
				H = g.ColorEditSavedHue;
			value_changed = value_changed_sv = true;
		}

		// Hue bar logic
		SetCursorScreenPos(ImVec2(bar0_pos_x, bar_pos.y));
		InvisibleButton("hue", ImVec2(sv_picker_size, sv_bar_size));
		if (IsItemActive())
		{
			H = 1.f - ImSaturate((io.MousePos.x - bar_pos.x) / (sv_picker_size - 1));
			value_changed = value_changed_h = true;
		}

		// Alpha bar logic
		if (alpha_bar)
		{
			SetCursorScreenPos(ImVec2(bar1_pos_x, bar_pos.y + 16));
			InvisibleButton("alpha", ImVec2(bars_width, sv_bar_size));
			if (IsItemActive())
			{
				col[3] = ImSaturate((io.MousePos.x - bar_pos.x + 5) / (bars_width - 1));
				value_changed = true;
			}
		}
		else
			col[3] = 1.f;

		PopItemFlag(); // ImGuiItemFlags_NoNav

		// Convert back color to RGB
		if (value_changed_h || value_changed_sv)
		{
			if (flags & ImGuiColorEditFlags_InputRGB)
			{
				ColorConvertHSVtoRGB(H, S, V, col[0], col[1], col[2]);
				g.ColorEditSavedHue = H;
				g.ColorEditSavedSat = S;
				g.ColorEditSavedColor = ColorConvertFloat4ToU32(ImVec4(col[0], col[1], col[2], 0));
			}

			else if (flags & ImGuiColorEditFlags_InputHSV)
			{
				col[0] = H;
				col[1] = S;
				col[2] = V;
			}
		}

		// R,G,B and H,S,V slider color editor
		bool value_changed_fix_hue_wrap = false;

		if (value_changed_fix_hue_wrap && (flags & ImGuiColorEditFlags_InputRGB))
		{
			// Try to cancel hue wrap (after ColorEdit4 call), if any
			float new_H, new_S, new_V;
			ColorConvertRGBtoHSV(col[0], col[1], col[2], new_H, new_S, new_V);
			if (new_H <= 0 && H > 0)
			{
				if (new_V <= 0 && V != new_V)
					ColorConvertHSVtoRGB(H, S, new_V <= 0 ? V * 0.5f : new_V, col[0], col[1], col[2]);
				else if (new_S <= 0)
					ColorConvertHSVtoRGB(H, new_S <= 0 ? S * 0.5f : new_S, new_V, col[0], col[1], col[2]);
			}
		}

		if (value_changed)
		{
			if (flags & ImGuiColorEditFlags_InputRGB)
			{
				R = col[0];
				G = col[1];
				B = col[2];
				ColorConvertRGBtoHSV(R, G, B, H, S, V);
				ColorEditRestoreHS(col, &H, &S, &V);   // Fix local Hue as display below will use it immediately.
			}
			else if (flags & ImGuiColorEditFlags_InputHSV)
			{
				H = col[0];
				S = col[1];
				V = col[2];
				ColorConvertHSVtoRGB(H, S, V, R, G, B);
			}
		}
		ImU32 user_col32_striped_of_alpha = ColorConvertFloat4ToU32(ImVec4(R, G, B, style.Alpha)); // Important: this is still including the main rendering/style alpha!!

		const int style_alpha8 = IM_F32_TO_INT8_SAT(style.Alpha);
		const ImU32 col_black = IM_COL32(0, 0, 0, style_alpha8);
		const ImU32 col_white = IM_COL32(255, 255, 255, style_alpha8);
		const ImU32 col_midgrey = IM_COL32(128, 128, 128, style_alpha8);
		const ImU32 col_hues[7] = { IM_COL32(255,0,0,style_alpha8), IM_COL32(255,0,255,style_alpha8), IM_COL32(0,0,255,style_alpha8),IM_COL32(0,255,255,style_alpha8), IM_COL32(0,255,0,style_alpha8), IM_COL32(255,255,0,style_alpha8), IM_COL32(255,0,0,style_alpha8) };

		ImVec4 hue_color_f(1, 1, 1, style.Alpha); ColorConvertHSVtoRGB(H, 1, 1, hue_color_f.x, hue_color_f.y, hue_color_f.z);
		ImU32 hue_color32 = ColorConvertFloat4ToU32(hue_color_f);

		ImVec2 sv_cursor_pos;

		// Render SV Square
		const int vtx_idx_0 = draw_list->VtxBuffer.Size;
		draw_list->AddRectFilled(picker_pos, picker_pos + ImVec2(sv_picker_size, sv_picker_size - 2), col_white, 4.0f);
		const int vtx_idx_1 = draw_list->VtxBuffer.Size;
		ShadeVertsLinearColorGradientKeepAlpha(draw_list, vtx_idx_0, vtx_idx_1, picker_pos, picker_pos + ImVec2(sv_picker_size, 0.0f), col_white, hue_color32);

		draw_list->AddRectFilledMultiColor(picker_pos, picker_pos + ImVec2(sv_picker_size, sv_picker_size), 0, 0, col_black, col_black, 4.f);

		sv_cursor_pos.x = ImClamp(IM_ROUND(picker_pos.x + ImSaturate(S) * sv_picker_size), picker_pos.x, picker_pos.x + sv_picker_size - 2); // Sneakily prevent the circle to stick out too much
		sv_cursor_pos.y = ImClamp(IM_ROUND(picker_pos.y + ImSaturate(1 - V) * (sv_picker_size)), picker_pos.y + 2, picker_pos.y + sv_picker_size);

		static std::map<ImGuiID, picker_state> anim;
		auto it_anim = anim.find(ImGui::GetID(label));

		if (it_anim == anim.end())
		{
			anim.insert({ ImGui::GetID(label), picker_state() });
			it_anim = anim.find(ImGui::GetID(label));
		}

		for (int i = 0; i < 6; ++i)
			GetForegroundDrawList()->AddRectFilledMultiColor(ImVec2(picker_pos.x + i * (sv_picker_size / 6) - (i == 5 ? 1 : 0), picker_pos.y + sv_picker_size + 8), ImVec2(picker_pos.x + (i + 1) * (sv_picker_size / 6) + (i == 0 ? 1 : 0), picker_pos.y + sv_picker_size + sv_bar_size - 7), col_hues[i], col_hues[i + 1], col_hues[i + 1], col_hues[i], 10.f, i == 0 ? ImDrawFlags_RoundCornersLeft : i == 5 ? ImDrawFlags_RoundCornersRight : ImDrawFlags_RoundCornersNone);

		float bar0_line_x = IM_ROUND(bar_pos.x + (1.f - H) * sv_picker_size);

		bar0_line_x = ImClamp(bar0_line_x, picker_pos.x + 12, picker_pos.x + sv_picker_size - 10);

		it_anim->second.hue_bar = ImLerp(it_anim->second.hue_bar, bar0_line_x - bar_pos.x, g.IO.DeltaTime * 24.f);

		for(int i = 0; i < 2;i++)
			GetForegroundDrawList()->AddShadowCircle(ImVec2(it_anim->second.hue_bar + bar_pos.x, bar_pos.y + 15), 5.f, ImColor(0, 0, 0, 255), 25.f, ImVec2(0, 0), ImDrawFlags_ShadowCutOutShapeBackground);
		GetForegroundDrawList()->AddCircle(ImVec2(it_anim->second.hue_bar + bar_pos.x, bar_pos.y + 15), 5.f, ImColor(255, 255, 255, 255), 30.f, 2.f);

		it_anim->second.circle_move = ImLerp(it_anim->second.circle_move, sv_cursor_pos - bar_pos, g.IO.DeltaTime * 24.f);
		it_anim->second.circle = ImLerp(it_anim->second.circle, value_changed_sv ? 4.f : 7.f, g.IO.DeltaTime * 24.f);

		GetForegroundDrawList()->AddCircle(it_anim->second.circle_move + bar_pos + ImVec2(0, 1), it_anim->second.circle, ImColor(255, 255, 255, 255), 32);

		if (alpha_bar)
		{
			float alpha = ImSaturate(col[3]);
			ImRect bar1_bb(bar1_pos_x, bar_pos.y + 20, bar1_pos_x + bars_width, bar_pos.y + 20 + sv_bar_size);

			draw_list->AddRectFilledMultiColor(picker_pos + ImVec2(0, 161), picker_pos + ImVec2(bars_width, 147 + sv_bar_size), col_black, user_col32_striped_of_alpha, user_col32_striped_of_alpha, col_black, 10.f);

			float bar1_line_x = IM_ROUND(bar_pos.x + alpha * bars_width);

			bar1_line_x = ImClamp(bar1_line_x, bar_pos.x, picker_pos.x + 200.f);
			it_anim->second.alpha_bar = ImLerp(it_anim->second.alpha_bar, bar1_line_x - bar_pos.x + 5.f, g.IO.DeltaTime * 24.f);
			GetForegroundDrawList()->AddCircleFilled(ImVec2(it_anim->second.alpha_bar + bar_pos.x, bar1_bb.Min.y + 11.0f), 6.5f, ImColor(255, 255, 255, 255), 100.f);
		}

		EndGroup();

		if (value_changed && memcmp(backup_initial_col, col, components * sizeof(float)) == 0) value_changed = false;
		if (value_changed) MarkItemEdited(g.LastItemData.ID);

		PopID();
		return value_changed;
	}

	struct color_edit_state {
		ImVec4 background;
		ImVec4 grab_color;
		ImVec4 text_color;
		float highlight_alpha;
		float grab_position; // Текущая позиция ползунка
		float target_position; // Целевая позиция ползунка
		float text_alpha; // Анимация прозрачности текста
	};

	struct coloreditor_state {
		bool actived;
	};
	bool ColorSlider(const char* label, float col[4], ImGuiColorEditFlags flags) {
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems) return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);

		const float element_width = 500.f - style.FramePadding.x;
		const float slider_height = 8.0f;
		const float grab_size = 12.0f;

		ImVec2 pos = window->DC.CursorPos;
		ImRect element_bb(pos, pos + ImVec2(element_width, 40));
		ImRect slider_bb(ImVec2(element_bb.Min.x + 10, element_bb.GetCenter().y - slider_height / 2),
			ImVec2(element_bb.Max.x - 10, element_bb.GetCenter().y + slider_height / 2));

		ImGui::ItemSize(ImVec2(element_width, slider_bb.GetHeight() + 24));
		if (!ImGui::ItemAdd(slider_bb, id)) return false;

		static std::map<ImGuiID, color_edit_state> anim;
		color_edit_state& state = anim[id];

		ImU32 col_hues[7] = {
			IM_COL32(255, 0, 0, 255),
			IM_COL32(255, 255, 0, 255),
			IM_COL32(0, 255, 0, 255),
			IM_COL32(0, 255, 255, 255),
			IM_COL32(0, 0, 255, 255),
			IM_COL32(255, 0, 255, 255),
			IM_COL32(255, 0, 0, 255)
		};

		const float section_width = (slider_bb.Max.x - slider_bb.Min.x) / 6.0f;

		for (int i = 0; i < 6; ++i) {
			ImVec2 section_min = ImVec2(slider_bb.Min.x + i * section_width, slider_bb.Min.y);
			ImVec2 section_max = ImVec2(slider_bb.Min.x + (i + 1) * section_width, slider_bb.Max.y);

			window->DrawList->AddRectFilledMultiColor(section_min, section_max,
				col_hues[i], col_hues[i + 1],
				col_hues[i + 1], col_hues[i]);
		}

		window->DrawList->AddRect(slider_bb.Min, slider_bb.Max, c::stroke_color, style.FrameRounding);

		bool hovered = false, held = false;
		bool pressed = ImGui::ButtonBehavior(slider_bb, id, &hovered, &held);

		if (held) {
			float mouse_pos = ImGui::GetMousePos().x - slider_bb.Min.x;
			state.target_position = ImClamp(mouse_pos - grab_size * 0.5f, 0.0f, (slider_bb.Max.x - slider_bb.Min.x) - grab_size);
		}

		state.grab_position = ImLerp(state.grab_position, state.target_position, g.IO.DeltaTime * 15.0f);

		ImVec2 grab_pos_min = ImVec2(slider_bb.Min.x + state.grab_position, slider_bb.Min.y - 2);
		ImVec2 grab_pos_max = ImVec2(slider_bb.Min.x + state.grab_position + grab_size, slider_bb.Max.y + 2);

		state.grab_color = ImLerp(state.grab_color, ImVec4(col[0], col[1], col[2], 1.0f), g.IO.DeltaTime * 8.0f);
		window->DrawList->AddRectFilled(grab_pos_min, grab_pos_max, ImGui::GetColorU32(state.grab_color), style.FrameRounding);
		window->DrawList->AddRect(grab_pos_min, grab_pos_max, ImGui::GetColorU32(ImVec4(0.0f, 0.0f, 0.0f, 0.2f)), style.FrameRounding);

		float relative_position = (state.grab_position) / (slider_bb.Max.x - slider_bb.Min.x - grab_size);
		float H = relative_position;
		float S = 1.0f;
		float V = 1.0f;

		ImGui::ColorConvertHSVtoRGB(H, S, V, col[0], col[1], col[2]);

		return pressed;
	}


	custom_popup editor_popup("EditorPopup");


	bool ColorEditor(const char* label, ImColor* col, ImGuiColorEditFlags flags) {
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems) return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);

		const float w = ImGui::GetContentRegionAvail().x - style.FramePadding.x;
		const ImVec2 pos = window->DC.CursorPos;

		const ImRect total_bb(pos, pos + ImVec2(w, 39));
		const ImRect color_box_bb(total_bb.Max - ImVec2(23.f, total_bb.GetSize().y - 8.f), total_bb.Max - ImVec2(0.f, 8.f));

		ItemSize(total_bb, 0.f);
		if (!ItemAdd(total_bb, id)) return false;

		bool hovered, held, pressed = ButtonBehavior(total_bb, id, &hovered, &held);

		static std::map<ImGuiID, coloreditor_state> anim;
		auto it_anim = anim.emplace(id, coloreditor_state()).first;

		if (IsItemClicked()) {
			it_anim->second.actived = !it_anim->second.actived;
			editor_popup.open(label);
			MarkItemEdited(id);
		}

		draw_animated_text(ImVec2(pos.x + 10.5f, utils::center_text(total_bb.Min, total_bb.Max, label).y), id, editor_popup.is_open(label), hovered, label);

		window->DrawList->AddRectFilled(color_box_bb.Min, color_box_bb.Max, GetColorU32(col->Value), style.FrameRounding);

		if (editor_popup.is_open(label)) {
			ImGui::SetNextWindowPos(ImGui::GetMousePos(), ImGuiCond_Once);
			if (editor_popup.begin(40.f, label)) {
				if (combo_popup.animated_button(ICON_CLOSE_LINE, ImDrawFlags_RoundCornersLeft)) {
					combo_popup.close();
					editor_popup.close();
				}
				ImGui::SameLine();

				ColorSlider(label, (float*)&col->Value, 0);

				editor_popup.end();
			}
		}

		if (!ImGui::IsAnyItemHovered() && ImGui::IsMouseClicked(0)) {
			editor_popup.close();
		}

		return pressed;
	}
	struct subtab_state {
		ImVec4 text_col;
		ImVec4 frame_col;
		ImVec4 line_col;
		float shader_alpha;
	};

	bool SubTab(const char* label, int* v, int number)
	{
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);

		const float square_sz = GetFrameHeight();
		const ImVec2 pos = window->DC.CursorPos;
		const ImRect total_bb(pos, pos + ImVec2(35 + CalcTextSize(label).x, 40));


		ItemSize(total_bb, style.FramePadding.y);
		ItemAdd(total_bb, id);

		static std::map<ImGuiID, subtab_state> anim;
		auto it_anim = anim.find(id);

		if (it_anim == anim.end())
		{
			anim.insert({ id, subtab_state() });
			it_anim = anim.find(id);
		}

		bool hovered, held;
		bool pressed = ButtonBehavior(total_bb, id, &hovered, &held);
		if (pressed)
		{
			*v = number;
		}

		RenderNavHighlight(total_bb, id);

		it_anim->second.frame_col = ImLerp(it_anim->second.frame_col, *v == number ? c::main_color : utils::GetColorWithAlpha(c::main_color, 0.f), GetAnimSpeed());
		it_anim->second.text_col = ImLerp(it_anim->second.text_col, *v == number ? c::label::active : c::label::regular, GetAnimSpeed());
		it_anim->second.text_col = ImLerp(it_anim->second.text_col, *v == number ? c::label::active : c::label::regular, GetAnimSpeed());
		it_anim->second.shader_alpha = ImLerp(it_anim->second.shader_alpha, *v == number ? style.Alpha : 0.f, GetAnimSpeed());

		shaderrt::Draw(ImGui::GetWindowDrawList(), total_bb.Min, total_bb.Max, c::elements::rounding, it_anim->second.shader_alpha, ImShaderTex_WindowBg);

		GetWindowDrawList()->AddRectFilled(total_bb.Min, total_bb.Max, utils::GetColorWithAlpha(c::second_color, c::second_color.Value.w * it_anim->second.shader_alpha), c::elements::rounding);
		GetWindowDrawList()->AddRect(total_bb.Min, total_bb.Max, utils::GetColorWithAlpha(c::stroke_color, c::stroke_color.Value.w * it_anim->second.shader_alpha), c::elements::rounding);
		
		ImRect line_bb(ImVec2(total_bb.GetCenter().x - 10.f, total_bb.Max.y - 3.f), ImVec2(total_bb.GetCenter().x + 10.f, total_bb.Max.y));

		//		window->DrawList->AddRectFilled(ImVec2(total_bb.GetCenter().x - 10.f, total_bb.Max.y - 3.f), ImVec2(total_bb.GetCenter().x + 10.f, total_bb.Max.y), GetColorU32(it_anim->second.line_col), style.FrameRounding);

		window->DrawList->AddShadowRect(line_bb.Min, line_bb.Max, GetColorU32(it_anim->second.frame_col), 45.f, ImVec2(0, 0));
		window->DrawList->AddRectFilled(line_bb.Min, line_bb.Max, GetColorU32(it_anim->second.frame_col), style.FrameRounding);


		window->DrawList->AddText(utils::center_text(total_bb.Min, total_bb.Max, label), GetColorU32(it_anim->second.text_col), label);


		return pressed;
	}

	bool ColorButton(const char* desc_id, const ImVec4& col, ImGuiColorEditFlags flags, const ImVec2& size_arg)
	{
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiID id = window->GetID(desc_id);
		const float default_size = GetFrameHeight();
		const ImVec2 size(21, 21);

		const float width = 50;
		const ImRect rect(window->DC.CursorPos, window->DC.CursorPos + ImVec2(width, 32));
		ImRect total_bb(ImVec2(rect.Max.x - 20, rect.Min.y + 6), rect.Max - ImVec2(0, 6));

		ItemSize(total_bb, (size.y >= default_size) ? g.Style.FramePadding.y : 0.0f);
		if (!ItemAdd(total_bb, id))
			return false;

		bool hovered, held;
		bool pressed = ButtonBehavior(total_bb, id, &hovered, &held);

		if (flags & ImGuiColorEditFlags_NoAlpha)
			flags &= ~(ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaPreviewHalf);

		ImVec4 col_rgb = col;
		if (flags & ImGuiColorEditFlags_InputHSV)
			ColorConvertHSVtoRGB(col_rgb.x, col_rgb.y, col_rgb.z, col_rgb.x, col_rgb.y, col_rgb.z);

		ImVec4 col_rgb_without_alpha(col_rgb.x, col_rgb.y, col_rgb.z, 1.0f);
		float grid_step = ImMin(size.x, size.y) / 2.99f;
		float rounding = ImMin(g.Style.FrameRounding, grid_step * 0.5f);
		ImRect bb_inner = total_bb;
		float off = 0.0f;
		if ((flags & ImGuiColorEditFlags_NoBorder) == 0)
		{
			off = -0.75f; // The border (using Col_FrameBg) tends to look off when color is near-opaque and rounding is enabled. This offset seemed like a good middle ground to reduce those artifacts.
			bb_inner.Expand(off);
		}

		ImVec4 col_source = col_rgb;

		RenderColorRectWithAlphaCheckerboard(window->DrawList, total_bb.Min, total_bb.Max, ImColor(1.f, 1.f, 1.f, 0.1f), 6.f, ImVec2(0, 0), c::elements::rounding);

		//window->DrawList->AddRectFilled(bb.Min, bb.Max , background_color, c::elements::rounding);

		window->DrawList->AddRectFilled(total_bb.Min, total_bb.Max, GetColorU32(col_rgb), c::elements::rounding);

		window->DrawList->AddRect(total_bb.Min, total_bb.Max, stroke_color, c::elements::rounding);

		RenderNavHighlight(total_bb, id);

		// Drag and Drop Source
		// NB: The ActiveId test is merely an optional micro-optimization, BeginDragDropSource() does the same test.
		if (g.ActiveId == id && !(flags & ImGuiColorEditFlags_NoDragDrop) && BeginDragDropSource())
		{
			if (flags & ImGuiColorEditFlags_NoAlpha)
				SetDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_3F, &col_rgb, sizeof(float) * 3, ImGuiCond_Once);
			else
				SetDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_4F, &col_rgb, sizeof(float) * 4, ImGuiCond_Once);
			ColorButton(desc_id, col, flags);
			SameLine();
			TextEx("Color");
			EndDragDropSource();
		}


		return pressed;
	}

	struct knob_state {
		float plus_float;
		int plus_int;
		ImVec4 background, circle, text;
		float slow_anim, circle_anim;
		float position;
	};


	bool KnobScalar(const char* label, ImGuiDataType data_type, void* p_data, const void* p_min, const void* p_max, const char* format, ImGuiSliderFlags flags)
	{
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems) return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);
		const float w = GetContentRegionMax().x - style.WindowPadding.x;

		const ImVec2 label_size = CalcTextSize(label, NULL, true);

		const ImRect frame_bb(window->DC.CursorPos + ImVec2(0, 0), window->DC.CursorPos + ImVec2(w, 32));

		const ImRect slider_bb(window->DC.CursorPos + ImVec2(w - 30, 0), window->DC.CursorPos + ImVec2(w, 100));

		const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? label_size.x : 0.0f, 0.0f));

		const bool temp_input_allowed = (flags & ImGuiSliderFlags_NoInput) == 0;
		ItemSize(ImRect(total_bb.Min, total_bb.Max - ImVec2(0, 0)));

		if (!ItemAdd(total_bb, id, &frame_bb, temp_input_allowed ? ImGuiItemFlags_Inputable : 0)) return false;

		if (format == NULL) format = DataTypeGetInfo(data_type)->PrintFmt;

		bool hovered = ItemHoverable(frame_bb, id, g.LastItemData.InFlags), held, pressed = ButtonBehavior(frame_bb, id, &hovered, &held, NULL);

		ImRect grab_bb;

		static std::map<ImGuiID, knob_state> anim;
		auto it_anim = anim.find(id);

		if (it_anim == anim.end())
		{
			anim.insert({ id, knob_state() });
			it_anim = anim.find(id);
		}

		it_anim->second.circle_anim = ImLerp(it_anim->second.circle_anim, IsItemActive() ? 11.f : 10.f, g.IO.DeltaTime * 6.f);

		if ((flags & ImGuiSliderFlags_Integer) == 0) {
			it_anim->second.plus_float = ImLerp(it_anim->second.plus_float, *(float*)p_data <= *(float*)p_max && hovered && GetAsyncKeyState(VK_OEM_PLUS) & 0x01 ? *(float*)p_data += 0.05f : *(float*)p_data >= *(float*)p_min && hovered && GetAsyncKeyState(VK_OEM_MINUS) & 0x01 ? *(float*)p_data -= 0.05f : 0, g.IO.DeltaTime * 6.f);
			if (*(float*)p_data > *(float*)p_max) *(float*)p_data = *(float*)p_max;
			if (*(float*)p_data < *(float*)p_min) *(float*)p_data = *(float*)p_min;
		}
		else
		{
			it_anim->second.plus_int = ImLerp(it_anim->second.plus_int, *(int*)p_data <= *(int*)p_max && hovered && GetAsyncKeyState(VK_OEM_PLUS) & 0x01 ? *(int*)p_data += 1 : *(int*)p_data >= *(int*)p_min && hovered && GetAsyncKeyState(VK_OEM_MINUS) & 0x01 ? *(int*)p_data -= 1 : 0, g.IO.DeltaTime * 6.f);
			if (*(int*)p_data > *(int*)p_max) *(int*)p_data = *(int*)p_max;
			if (*(int*)p_data < *(int*)p_min) *(int*)p_data = *(int*)p_min;
		}

		it_anim->second.text = ImLerp(it_anim->second.text, g.ActiveId == id ? c::text::text_active : hovered ? c::text::text_hov : c::text::text, g.IO.DeltaTime * 6.f);

		const bool value_changed = DragBehavior(id, data_type, p_data, 0.f, p_min, p_max, format, NULL);


		if (value_changed) MarkItemEdited(id);

		char value_buf[64];
		const char* value_buf_end = value_buf + DataTypeFormatString(value_buf, IM_ARRAYSIZE(value_buf), data_type, p_data, format);

		float radius = 10.f;
		float thickness = 3.f;

		it_anim->second.position = ImLerp(it_anim->second.position, *static_cast<float*>(p_data) / *reinterpret_cast<const float*>(p_max) * 6.25f, ImGui::GetIO().DeltaTime * 18.f);

		GetWindowDrawList()->PathClear();
		GetWindowDrawList()->PathArcTo(ImVec2(frame_bb.Max.x + radius - 22.f, frame_bb.Min.y + (32 / 2)), radius, IM_PI * 1.5f, IM_PI * 1.5f + it_anim->second.position, 40.f);
		GetWindowDrawList()->PathStroke(GetColorU32(c::accent), 0, thickness);

		GetWindowDrawList()->AddCircleFilled(ImVec2(frame_bb.Max.x + radius - 22.f + ImCos(IM_PI * 1.5f + it_anim->second.position) * radius, frame_bb.Min.y + (32 / 2) + ImSin(IM_PI * 1.5f + it_anim->second.position) * radius), 2.f, GetColorU32(c::text::text_active));

		GetWindowDrawList()->AddText(ImVec2(frame_bb.Max.x - (40 + CalcTextSize(value_buf).x), frame_bb.Min.y + (32 - CalcTextSize(value_buf).y) / 2), GetColorU32(c::text::text), value_buf);

		GetWindowDrawList()->AddText(ImVec2(frame_bb.Max.x - w, frame_bb.Min.y + (32 - CalcTextSize(value_buf).y) / 2), GetColorU32(it_anim->second.text), label);

		return value_changed;
	}

	bool KnobFloat(const char* label, float* v, float v_min, float v_max, const char* format, ImGuiSliderFlags flags)
	{
		return KnobScalar(label, ImGuiDataType_Float, v, &v_min, &v_max, format, flags);
	}

	bool KnobInt(const char* label, int* v, int v_min, int v_max, const char* format, ImGuiSliderFlags flags)
	{
		return KnobScalar(label, ImGuiDataType_S32, v, &v_min, &v_max, format, flags | ImGuiSliderFlags_Integer);
	}

	struct slider_state {
		ImVec4 background, circle, text, highlighting, grab_col, frame_col;
		ImVec2 grab_offset;
		float position, slow, value_width;
		bool anim_active;
		float wanted_value, swap_speed;
		const char* current_text;
	};
	
	bool SliderScalar(const char* label, ImGuiDataType data_type, void* p_data, const void* p_min, const void* p_max, const char* format, ImGuiSliderFlags flags)
	{
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems) return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);
		const float w = GetContentRegionMax().x - style.WindowPadding.x;

		const ImVec2 label_size = CalcTextSize(label, NULL, true);

		const ImVec2 pos = window->DC.CursorPos;

		const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, 39.f));

		const ImRect total_bb(frame_bb.Min, frame_bb.Max);


		const bool temp_input_allowed = (flags & ImGuiSliderFlags_NoInput) == 0;
		ItemSize(ImRect(total_bb.Min, total_bb.Max));

		if (!ItemAdd(total_bb, id, &frame_bb, temp_input_allowed ? ImGuiItemFlags_Inputable : 0)) return false;

		if (format == NULL) format = DataTypeGetInfo(data_type)->PrintFmt;

		bool hovered = ItemHoverable(frame_bb, id, g.LastItemData.InFlags), held, pressed = ButtonBehavior(frame_bb, id, &hovered, &held, NULL);

		ImRect grab_bb;

		static std::map<ImGuiID, slider_state> anim;
		auto it_anim = anim.find(id);

		if (it_anim == anim.end())
		{
			anim.insert({ id, slider_state() });
			it_anim = anim.find(id);
		}

		char value_buf[64];
		const char* value_buf_end = value_buf + DataTypeFormatString(value_buf, IM_ARRAYSIZE(value_buf), data_type, p_data, format);

		ImGui::PushFont(font::medium_small);
		it_anim->second.value_width = ImLerp(it_anim->second.value_width, CalcTextSize(value_buf).x, GetAnimSpeed());
		ImRect value_bb = ImRect(total_bb.Max - ImVec2(20.5f + it_anim->second.value_width, 49), total_bb.Max - ImVec2(10.5f, 26));
		ImGui::PopFont(); 

		it_anim->second.text = ImLerp(it_anim->second.text, g.ActiveId == id || hovered ? c::text::label::active : hovered ? c::text::label::hovered : c::text::label::regular, GetAnimSpeed());

		it_anim->second.highlighting = ImLerp(it_anim->second.highlighting, g.ActiveId == id || hovered ? ImColor(1.f, 1.f, 1.f, 0.1f) : ImColor(1.f, 1.f, 1.f, 0.f), GetAnimSpeed() * 2);
		it_anim->second.grab_offset = ImLerp(it_anim->second.grab_offset, g.ActiveId == id || hovered ? ImVec2(4.f, 3.f) : ImVec2(4.f, 0.f), GetAnimSpeed() * 2.5f);
		it_anim->second.anim_active = abs(grab_bb.Min.x - (frame_bb.Min.x) - it_anim->second.slow) > 0.3f;
		it_anim->second.frame_col = ImLerp(it_anim->second.frame_col, (hovered || IsItemActive()) && it_anim->second.slow > 5.f ? c::main_color : utils::GetColorWithAlpha(c::main_color, 0.f), GetAnimSpeed());
		//it_anim->second.grab_col = ImLerp(it_anim->second.grab_col, ImGui::GetIO().DeltaTime * 8.f);

		const bool value_changed = SliderBehavior(ImRect(total_bb.Min - ImVec2(18, 0), total_bb.Max + ImVec2(0, 0)), id, data_type, p_data, p_min, p_max, format, flags, &grab_bb);

		
		if (value_changed) {
			MarkItemEdited(id);
			it_anim->second.wanted_value = grab_bb.Min.x - (frame_bb.Min.x);
		}
		std::string label_str = label;

		it_anim->second.slow = ImLerp(it_anim->second.slow, grab_bb.Max.x - (frame_bb.Min.x), g.IO.DeltaTime * 25.f);


		PushClipRect(total_bb.Min, ImVec2(it_anim->second.slow + (total_bb.Min.x), total_bb.Max.y), true);
		shaderrt::Draw(ImGui::GetWindowDrawList(), total_bb.Min, total_bb.Max, c::elements::rounding, shader_alpha, ImShaderTex_WindowBg);
		PopClipRect();

		grab_bb = ImRect(ImVec2(it_anim->second.slow + (total_bb.Min.x - 1.f), total_bb.Min.y), ImVec2(it_anim->second.slow + (total_bb.Min.x + 1), total_bb.Max.y));

		
		draw_volumetric_rect(total_bb.Min, total_bb.Max);

		
		PushClipRect(grab_bb.Min, total_bb.Max, true);
		draw_animated_text(ImVec2(pos.x + 10.5f, utils::center_text(total_bb.Min, total_bb.Max, label).y), id, false, false, label);
		float* float_ptr = static_cast<float*>(p_data);
		PopClipRect();

		PushClipRect(total_bb.Min, grab_bb.Max, true);
		window->DrawList->AddText(ImVec2(pos.x + 10.5f, utils::center_text(total_bb.Min, total_bb.Max, label).y), c::label::active, label);
		PopClipRect();


		// Получение значения float
		float value = *float_ptr;
		
		ImGui::PushFont(font::medium_small);
		//it_anim->second.current_text = roll_text.render_text(value_bb, *(int*)p_data, id);

		GetWindowDrawList()->AddText(ImVec2(value_bb.GetCenter().x - it_anim->second.value_width / 2, utils::center_text(total_bb.Min, total_bb.Max, value_buf).y), GetColorU32(it_anim->second.text), value_buf, value_buf_end);
		ImGui::PopFont();

		window->DrawList->AddShadowRect(grab_bb.Min, grab_bb.Max, GetColorU32(it_anim->second.frame_col), 45.f, ImVec2(0, 0));
		window->DrawList->AddRectFilled(grab_bb.Min, grab_bb.Max, GetColorU32(it_anim->second.frame_col), style.FrameRounding);


		return value_changed;
	}

	bool SliderFloat(const char* label, float* v, float v_min, float v_max, const char* format, ImGuiSliderFlags flags)
	{
		return SliderScalar(label, ImGuiDataType_Float, v, &v_min, &v_max, format, flags);
	}

	bool SliderInt(const char* label, int* v, int v_min, int v_max, const char* format, ImGuiSliderFlags flags)
	{
		return SliderScalar(label, ImGuiDataType_S32, v, &v_min, &v_max, format, flags);
	}

	bool ImConfig(const char* label, const char* date, const char* time, const char* creator)
	{
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);

		const float square_sz = GetFrameHeight();
		const ImVec2 pos = window->DC.CursorPos;
		const ImRect total_bb(pos, pos + ImVec2(ImGui::GetContentRegionAvail().x, 103));

		ItemSize(total_bb, style.FramePadding.y);
		ItemAdd(total_bb, id);

		bool hovered, held;
		bool pressed = ButtonBehavior(total_bb, id, &hovered, &held);

		RenderNavHighlight(total_bb, id);

		const char* label_end = FindRenderedTextEnd(label);

		window->DrawList->AddRectFilled(total_bb.Min, total_bb.Max, c::second_color, style.FrameRounding);

		window->DrawList->AddRectFilled(total_bb.Min, ImVec2(total_bb.Max.x, total_bb.Min.y + 35.f), c::second_color, style.FrameRounding, ImDrawFlags_RoundCornersBottom);

		window->DrawList->AddRect(total_bb.Min, total_bb.Max, stroke_color, style.FrameRounding);

		window->DrawList->AddText(ImVec2(total_bb.Min.x + 10.f, utils::center_text(total_bb.Min, ImVec2(total_bb.Max.x, total_bb.Min.y + 35.f), label).y), c::label::active, label, label_end);

		window->DrawList->AddText(ImVec2(total_bb.Min.x + 10.f, total_bb.Min.y + 45.f), c::label::active, "Status: ");

		window->DrawList->AddText(ImVec2(total_bb.Min.x + 10.f + CalcTextSize("Status: ").x, total_bb.Min.y + 45.f), c::main_color, date);

		window->DrawList->AddText(ImVec2(total_bb.Min.x + 10.f, total_bb.Min.y + 75.f), c::label::active, "Packages: ");

		window->DrawList->AddText(ImVec2(total_bb.Min.x + 10.f + CalcTextSize("Packages: ").x, total_bb.Min.y + 75.f), c::main_color, time);


		window->DrawList->AddText(ImVec2(total_bb.Max.x - 10.f - CalcTextSize("Obb: ").x - CalcTextSize(creator).x, utils::center_text(total_bb.Min, ImVec2(total_bb.Max.x, total_bb.Min.y + 35.f), "Obb: ").y), c::label::active, "Obb: ");
		window->DrawList->AddText(ImVec2(total_bb.Max.x - 10.f - CalcTextSize(creator).x, utils::center_text(total_bb.Min, ImVec2(total_bb.Max.x, total_bb.Min.y + 35.f), creator).y), c::label::regular, creator);

		window->DrawList->AddRectFilled(total_bb.Max - ImVec2(45, 51), total_bb.Max - ImVec2(45, 51) + ImVec2(35, 35), c::main_color, style.FrameRounding);

		window->DrawList->AddRect(total_bb.Max - ImVec2(45, 51), total_bb.Max - ImVec2(45, 51) + ImVec2(35, 35), stroke_color, style.FrameRounding, 0, 1.5f);


		window->DrawList->AddText(utils::center_text(total_bb.Max - ImVec2(45, 51), total_bb.Max - ImVec2(45, 51) + ImVec2(35, 35), ICON_PLAY_FILL), c::window_bg_color, ICON_PLAY_FILL);



		return pressed;
	}

}

