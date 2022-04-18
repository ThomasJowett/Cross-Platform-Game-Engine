#include "Fonts.h"

#include "imgui/imgui.h"

#include "Fonts/DroidSans.cpp"
#include "Fonts/Consolas.cpp"
#include "Fonts/fa-solid.cpp"
#include "Fonts/fa-webfont.cpp"
#include "Fonts/md-regular.cpp"
#include "Fonts/md-webfont.cpp"

#include "IconsFontAwesome5.h"
#include "IconsMaterialDesign.h"
#include "IconsMaterialDesignIcons.h"

ImFont* Fonts::Consolas = nullptr;
ImFont* Fonts::DroidSans = nullptr;
ImFont* Fonts::Icons = nullptr;
ImFont* Fonts::MDIcons = nullptr;

void Fonts::LoadFonts(float fontSize)
{
	ImGuiIO& io = ImGui::GetIO();
	DroidSans = io.Fonts->AddFontFromMemoryCompressedTTF(DroidSans_compressed_data, DroidSans_compressed_size, fontSize);

	static const ImWchar fa_icons_ranges[] = { (ImWchar)ICON_MIN_FA, (ImWchar)ICON_MAX_FA, 0 };
	static const ImWchar md_icons_ranges[] = { (ImWchar)ICON_MIN_MD, (ImWchar)ICON_MAX_MD, 0 };
	static const ImWchar mdi_icons_ranges[] = { (ImWchar)ICON_MIN_MDI, (ImWchar)ICON_MAX_MDI, 0 };
	ImFontConfig icons_config;
	icons_config.MergeMode = true;
	icons_config.PixelSnapH = true;
	icons_config.OversampleH = icons_config.OversampleV = 1;


	icons_config.GlyphOffset.y = 1.0f;
	icons_config.SizePixels = fontSize;
	io.Fonts->AddFontFromMemoryCompressedTTF(fa_solid_compressed_data, fa_solid_compressed_size, fontSize, &icons_config, fa_icons_ranges);
	io.Fonts->AddFontFromMemoryCompressedTTF(fa_webfont_compressed_data, fa_webfont_compressed_size, fontSize, &icons_config, fa_icons_ranges);

	icons_config.GlyphOffset.y = 2.0f;
	icons_config.SizePixels = fontSize + 4.0f;
	io.Fonts->AddFontFromMemoryCompressedTTF(md_webfont_compressed_data, md_webfont_compressed_size, fontSize + 4.0f, &icons_config, mdi_icons_ranges);

	//icons_config.GlyphOffset.y += 4.0f;
	//io.Fonts->AddFontFromMemoryCompressedTTF(md_regular_compressed_data, md_regular_compressed_size, fontSize + 4.0f, &icons_config, md_icons_ranges);

	//icons_config.MergeMode = false;
	//icons_config.GlyphOffset.y = 0.0f;

	Consolas = io.Fonts->AddFontFromMemoryCompressedTTF(Consolas_compressed_data, Consolas_compressed_size, fontSize);

	ImFontConfig icons_large_config;
	icons_large_config.MergeMode = false;
	icons_large_config.PixelSnapH = true;
	icons_large_config.OversampleH = icons_large_config.OversampleV = 1;
	icons_large_config.SizePixels = 72.0f;

	Icons = io.Fonts->AddFontFromMemoryCompressedTTF(fa_solid_compressed_data, fa_solid_compressed_size, 72.0f, &icons_large_config, fa_icons_ranges);
	io.Fonts->AddFontFromMemoryCompressedTTF(md_regular_compressed_data, md_regular_compressed_size, 72.0f, &icons_large_config, md_icons_ranges);
}
