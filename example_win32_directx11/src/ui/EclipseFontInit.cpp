#include "EclipseFontInit.hpp"

#include <EclipseFontPack.h>
#include <custom_logo.hpp>
#include <src/Fonts/Fonts.hpp>
#include <src/Fonts/icon.h>
#include <UIStubs.hpp>
#include <imgui_settings.h>
#include <ImGui/font_defines.h>
#include <imgui_freetype.h>
#include <D3DX11tex.h>

namespace texture {
    extern ID3D11ShaderResourceView* custom_logo;
}

namespace YorzenUI {

void LoadEclipseFonts(ID3D11Device* device)
{
    ImGuiIO& io = ImGui::GetIO();

    ImFontConfig cfg;
    cfg.FontBuilderFlags = ImGuiFreeTypeBuilderFlags_MonoHinting | ImGuiFreeTypeBuilderFlags_LoadColor;

    io.Fonts->AddFontFromMemoryTTF(inter_semibold, sizeof(inter_semibold), 16.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());

    static ImWchar icomoon_ranges[] = { 0x1, 0x10FFFD, 0 };
    static ImFontConfig icomoon_config;
    icomoon_config.OversampleH = icomoon_config.OversampleV = 1;
    icomoon_config.MergeMode = true;
    icomoon_config.GlyphOffset.y = 6.5f;
    icomoon_config.FontBuilderFlags |= ImGuiFreeTypeBuilderFlags_LoadColor;
    io.Fonts->AddFontFromMemoryCompressedBase85TTF(icomoon_compressed_data_base85, 25.f, &icomoon_config, icomoon_ranges);

    font::esp_font = io.Fonts->AddFontFromMemoryTTF(SFProDisplayRegular, sizeof(SFProDisplayRegular), 15.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
    font::description_font = io.Fonts->AddFontFromMemoryTTF(::InterMedium, sizeof(::InterMedium), 15.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
    font::regular_m = io.Fonts->AddFontFromMemoryTTF(SFProDisplayRegular, sizeof(SFProDisplayRegular), 21.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
    font::regular_l = io.Fonts->AddFontFromMemoryTTF(SFProDisplayRegular, sizeof(SFProDisplayRegular), 41.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
    font::s_inter_semibold = io.Fonts->AddFontFromMemoryTTF(inter_semibold, sizeof(inter_semibold), 17.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
    font::inter_semibold = io.Fonts->AddFontFromMemoryTTF(inter_semibold, sizeof(inter_semibold), 29.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());

    icomoon_config.MergeMode = true;
    icomoon_config.GlyphOffset.y = 6.5f;
    io.Fonts->AddFontFromMemoryCompressedBase85TTF(icomoon_compressed_data_base85, 29.f, &icomoon_config, icomoon_ranges);

    font::small_font = io.Fonts->AddFontFromMemoryTTF(inter_semibold, sizeof(inter_semibold), 14.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
    font::inter_regular = io.Fonts->AddFontFromMemoryTTF(SFProDisplayRegular, sizeof(SFProDisplayRegular), 15.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
    font::inter_medium = io.Fonts->AddFontFromMemoryTTF(::InterMedium, sizeof(::InterMedium), 17.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
    font::icomoon_page = io.Fonts->AddFontFromMemoryTTF(::icomoon_page, sizeof(::icomoon_page), 28.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
    font::icomoon_logo = io.Fonts->AddFontFromMemoryTTF(::icomoon_page, sizeof(::icomoon_page), 30.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
    font::icon_notify = io.Fonts->AddFontFromMemoryTTF(::icon_notify, sizeof(::icon_notify), 17.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());

    ImFontConfig tabIconCfg;
    tabIconCfg.OversampleH = tabIconCfg.OversampleV = 1;
    font::icomoon_tabs = io.Fonts->AddFontFromMemoryCompressedBase85TTF(icomoon_compressed_data_base85, 25.f, &tabIconCfg, icomoon_ranges);
    font::icon_child = io.Fonts->AddFontFromMemoryCompressedBase85TTF(icomoon_compressed_data_base85, 30.f, &tabIconCfg, icomoon_ranges);
    font::bold_small = font::small_font;
    font::medium_small = font::description_font;
    font::inter_bold = font::inter_semibold;
    font::iconuwu = font::small_font;

    ImFontConfig weaponBaseCfg;
    weaponBaseCfg.SizePixels = 41.0f;
    FWork::Fonts::IconWeapon = io.Fonts->AddFontDefault(&weaponBaseCfg);
    ImFontConfig weaponMergeCfg;
    weaponMergeCfg.MergeMode = true;
    weaponMergeCfg.PixelSnapH = true;
    weaponMergeCfg.OversampleH = 1;
    weaponMergeCfg.OversampleV = 1;
    static const ImWchar weaponRanges[] = { 0xE000, 0xE204, 0 };
    io.Fonts->AddFontFromMemoryCompressedTTF(
        icon_compressed_data, icon_compressed_size, 41.0f, &weaponMergeCfg, weaponRanges);

    FWork::Fonts::LoadEspNameFonts();

    if (device && texture::custom_logo == nullptr) {
        D3DX11_IMAGE_LOAD_INFO info{};
        ID3DX11ThreadPump* pump = nullptr;
        D3DX11CreateShaderResourceViewFromMemory(
            device, custom_logo_bytes, sizeof(custom_logo_bytes), &info, pump, &texture::custom_logo, 0);
    }
}

}
