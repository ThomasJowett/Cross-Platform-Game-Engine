#include "EditorStyles.h"

#include "ImGui/ImGuiTextEditor.h"

#include "imgui_internal.h"

const std::vector<EditorStyles::Theme>& EditorStyles::GetThemes()
{
	static const std::vector<Theme> themes = {
		{ "Attercop", &EditorStyles::Attercop },
		{ "Cobalt", &EditorStyles::Cobalt },
		{ "Phthalo", &EditorStyles::Phthalo },
		{ "Majorelle", &EditorStyles::Majorelle },
		{ "Cherry", &EditorStyles::Cherry },
		{ "Xiketic", &EditorStyles::Xiketic },
		{ "Onyx", &EditorStyles::Onyx },
		{ "Mono", &EditorStyles::Mono },
		{ "Emerald", &EditorStyles::Emerald },
		{ "Ochre", &EditorStyles::Ochre },
	};
	return themes;
}

bool EditorStyles::Apply(const std::string& name)
{
	// Reset so colours a theme doesn't set aren't left over from the previous theme
	ImGui::StyleColorsDark();
	for (const Theme& theme : GetThemes())
	{
		if (name == theme.name)
			return theme.apply();
	}
	return Attercop();
}

bool EditorStyles::Attercop()
{
	ImVec4* colors = ImGui::GetStyle().Colors;

	// Logo colours
	const ImVec4 coral = ImVec4(1.00f, 0.376f, 0.486f, 1.00f);
	const ImVec4 sky = ImVec4(0.60f, 0.80f, 1.00f, 1.00f);
	const ImVec4 mint = ImVec4(0.37f, 0.86f, 0.54f, 1.00f);
	const ImVec4 navy = ImVec4(0.18f, 0.26f, 0.41f, 1.00f);

	const ImVec4 coralDeep = ImVec4(0.62f, 0.23f, 0.32f, 1.00f);
	const ImVec4 coralBright = ImVec4(1.00f, 0.52f, 0.61f, 1.00f);
	const ImVec4 navyBright = ImVec4(0.26f, 0.36f, 0.54f, 1.00f);
	const ImVec4 background = ImVec4(0.106f, 0.129f, 0.188f, 1.00f);
	const ImVec4 backgroundDark = ImVec4(0.082f, 0.102f, 0.149f, 1.00f);
	const ImVec4 frame = ImVec4(0.149f, 0.180f, 0.251f, 1.00f);

	colors[ImGuiCol_Text] = ImVec4(0.91f, 0.92f, 0.95f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.47f, 0.52f, 0.61f, 1.00f);
	colors[ImGuiCol_WindowBg] = background;
	colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.122f, 0.149f, 0.212f, 0.98f);
	colors[ImGuiCol_Border] = ImVec4(navy.x, navy.y, navy.z, 0.70f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg] = frame;
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.20f, 0.25f, 0.35f, 1.00f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.24f, 0.31f, 0.45f, 1.00f);
	colors[ImGuiCol_TitleBg] = backgroundDark;
	colors[ImGuiCol_TitleBgActive] = coralDeep;
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(backgroundDark.x, backgroundDark.y, backgroundDark.z, 0.75f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.122f, 0.149f, 0.212f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(backgroundDark.x, backgroundDark.y, backgroundDark.z, 0.60f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.24f, 0.29f, 0.40f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.30f, 0.37f, 0.50f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = coral;
	colors[ImGuiCol_CheckMark] = mint;
	colors[ImGuiCol_SliderGrab] = coral;
	colors[ImGuiCol_SliderGrabActive] = coralBright;
	colors[ImGuiCol_Button] = navy;
	colors[ImGuiCol_ButtonHovered] = navyBright;
	colors[ImGuiCol_ButtonActive] = coralDeep;
	colors[ImGuiCol_Header] = ImVec4(coral.x, coral.y, coral.z, 0.28f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(coral.x, coral.y, coral.z, 0.45f);
	colors[ImGuiCol_HeaderActive] = ImVec4(coral.x, coral.y, coral.z, 0.65f);
	colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
	colors[ImGuiCol_SeparatorHovered] = ImVec4(coral.x, coral.y, coral.z, 0.70f);
	colors[ImGuiCol_SeparatorActive] = coral;
	colors[ImGuiCol_ResizeGrip] = ImVec4(navy.x, navy.y, navy.z, 0.60f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(coral.x, coral.y, coral.z, 0.70f);
	colors[ImGuiCol_ResizeGripActive] = coral;
	colors[ImGuiCol_Tab] = ImVec4(0.125f, 0.153f, 0.216f, 1.00f);
	colors[ImGuiCol_TabHovered] = ImVec4(coral.x, coral.y, coral.z, 0.55f);
	colors[ImGuiCol_TabActive] = coralDeep;
	colors[ImGuiCol_TabSelectedOverline] = coral;
	colors[ImGuiCol_TabUnfocused] = backgroundDark;
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.27f, 0.20f, 0.27f, 1.00f);
	colors[ImGuiCol_DockingPreview] = ImVec4(coral.x, coral.y, coral.z, 0.70f);
	colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.07f, 0.08f, 0.12f, 1.00f);
	colors[ImGuiCol_PlotLines] = sky;
	colors[ImGuiCol_PlotLinesHovered] = coral;
	colors[ImGuiCol_PlotHistogram] = mint;
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.55f, 0.93f, 0.68f, 1.00f);
	colors[ImGuiCol_TableHeaderBg] = frame;
	colors[ImGuiCol_TableBorderStrong] = navy;
	colors[ImGuiCol_TableBorderLight] = ImVec4(0.18f, 0.22f, 0.30f, 1.00f);
	colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.03f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(sky.x, sky.y, sky.z, 0.30f);
	colors[ImGuiCol_DragDropTarget] = sky;
	colors[ImGuiCol_NavHighlight] = sky;
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.05f, 0.06f, 0.09f, 0.50f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.05f, 0.06f, 0.09f, 0.60f);

	TextEditor::SetPalette(TextEditor::Palette({
	    0xffece8e6, // Default
	    0xff7c60ff, // Keyword
	    0xff89dc5e, // Number
	    0xffffcc99, // String
	    0xffffcc99, // Char literal
	    0xffd0cac6, // Punctuation
	    0xffea92c7, // Preprocessor
	    0xffe0dcd8, // Identifier
	    0xff6bcbff, // Known identifier
	    0xffea92c7, // Preproc identifier
	    0xff997a6b, // Comment (single line)
	    0xff997a6b, // Comment (multi line)
	    0xff271b16, // Background
	    0xff7c60ff, // Cursor
	    0x50ffcc99, // Selection
	    0x800020ff, // ErrorMarker
	    0x40f08000, // Breakpoint
	    0xff7a5d4f, // Line number
	    0x40000000, // Current line fill
	    0x40808080, // Current line fill (inactive)
	    0x40a0a0a0, // Current line edge
	}));

	return true;
}

bool EditorStyles::Cobalt()
{
	ImVec4* colors = ImGui::GetStyle().Colors;

	const ImVec4 blue = ImVec4(0.20f, 0.47f, 0.86f, 1.00f);
	const ImVec4 blueBright = ImVec4(0.30f, 0.57f, 0.95f, 1.00f);
	const ImVec4 blueDeep = ImVec4(0.12f, 0.30f, 0.60f, 1.00f);
	const ImVec4 orange = ImVec4(1.00f, 0.55f, 0.13f, 1.00f);
	const ImVec4 orangeBright = ImVec4(1.00f, 0.66f, 0.32f, 1.00f);

	colors[ImGuiCol_Text] = ImVec4(0.90f, 0.92f, 0.95f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.44f, 0.49f, 0.57f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.075f, 0.090f, 0.120f, 1.00f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.090f, 0.108f, 0.145f, 0.98f);
	colors[ImGuiCol_Border] = ImVec4(0.20f, 0.25f, 0.33f, 0.60f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.12f, 0.155f, 0.215f, 1.00f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.16f, 0.23f, 0.34f, 1.00f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.18f, 0.29f, 0.46f, 1.00f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.055f, 0.067f, 0.090f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.085f, 0.150f, 0.265f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.055f, 0.067f, 0.090f, 0.75f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.095f, 0.115f, 0.155f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.055f, 0.067f, 0.090f, 0.60f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.22f, 0.28f, 0.37f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.28f, 0.36f, 0.48f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = blue;
	colors[ImGuiCol_CheckMark] = orange;
	colors[ImGuiCol_SliderGrab] = blue;
	colors[ImGuiCol_SliderGrabActive] = orange;
	colors[ImGuiCol_Button] = ImVec4(blue.x, blue.y, blue.z, 0.55f);
	colors[ImGuiCol_ButtonHovered] = blueBright;
	colors[ImGuiCol_ButtonActive] = orange;
	colors[ImGuiCol_Header] = ImVec4(blue.x, blue.y, blue.z, 0.35f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(blue.x, blue.y, blue.z, 0.70f);
	colors[ImGuiCol_HeaderActive] = blue;
	colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
	colors[ImGuiCol_SeparatorHovered] = ImVec4(orange.x, orange.y, orange.z, 0.70f);
	colors[ImGuiCol_SeparatorActive] = orange;
	colors[ImGuiCol_ResizeGrip] = ImVec4(blue.x, blue.y, blue.z, 0.25f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(orange.x, orange.y, orange.z, 0.70f);
	colors[ImGuiCol_ResizeGripActive] = orange;
	colors[ImGuiCol_Tab] = ImVec4(0.10f, 0.135f, 0.19f, 1.00f);
	colors[ImGuiCol_TabHovered] = blueBright;
	colors[ImGuiCol_TabActive] = blueDeep;
	colors[ImGuiCol_TabSelectedOverline] = orange;
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.075f, 0.095f, 0.130f, 1.00f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.11f, 0.19f, 0.32f, 1.00f);
	colors[ImGuiCol_DockingPreview] = ImVec4(orange.x, orange.y, orange.z, 0.70f);
	colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.05f, 0.06f, 0.08f, 1.00f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.55f, 0.65f, 0.80f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = orange;
	colors[ImGuiCol_PlotHistogram] = orange;
	colors[ImGuiCol_PlotHistogramHovered] = orangeBright;
	colors[ImGuiCol_TextSelectedBg] = ImVec4(orange.x, orange.y, orange.z, 0.35f);
	colors[ImGuiCol_DragDropTarget] = orange;
	colors[ImGuiCol_NavHighlight] = orange;
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.05f, 0.06f, 0.08f, 0.50f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.05f, 0.06f, 0.08f, 0.60f);

	TextEditor::SetPalette(TextEditor::Palette({
	    0xffe6e3e0, // Default
	    0xffffa25c, // Keyword
	    0xff4da0ff, // Number
	    0xff73b6e6, // String
	    0xff73b6e6, // Char literal
	    0xffd0ccc8, // Punctuation
	    0xffc086c5, // Preprocessor
	    0xffdcd8d4, // Identifier
	    0xffb0c94e, // Known identifier
	    0xffc086c5, // Preproc identifier
	    0xff99816a, // Comment (single line)
	    0xff99816a, // Comment (multi line)
	    0xff1f1813, // Background
	    0xff218dff, // Cursor
	    0x60218dff, // Selection
	    0x800020ff, // ErrorMarker
	    0x40f08000, // Breakpoint
	    0xff705a4a, // Line number
	    0x40000000, // Current line fill
	    0x40808080, // Current line fill (inactive)
	    0x40a0a0a0, // Current line edge
	}));

	return true;
}

bool EditorStyles::Phthalo()
{
	ImVec4* colors = ImGui::GetStyle().Colors;

	colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
	colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.16f, 0.29f, 0.48f, 0.54f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.16f, 0.29f, 0.48f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	colors[ImGuiCol_Tab] = ImLerp(colors[ImGuiCol_Header], colors[ImGuiCol_TitleBgActive], 0.80f);
	colors[ImGuiCol_TabHovered] = colors[ImGuiCol_HeaderHovered];
	colors[ImGuiCol_TabActive] = ImLerp(colors[ImGuiCol_HeaderActive], colors[ImGuiCol_TitleBgActive], 0.60f);
	colors[ImGuiCol_TabUnfocused] = ImLerp(colors[ImGuiCol_Tab], colors[ImGuiCol_TitleBg], 0.80f);
	colors[ImGuiCol_TabUnfocusedActive] = ImLerp(colors[ImGuiCol_TabActive], colors[ImGuiCol_TitleBg], 0.40f);
	colors[ImGuiCol_DockingPreview] = colors[ImGuiCol_HeaderActive] * ImVec4(1.0f, 1.0f, 1.0f, 0.7f);
	colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

	TextEditor::SetPalette(TextEditor::Palette({
	    0xffffffff, // Default
	    0xffd69c56, // Keyword
	    0xff00ff00, // Number
	    0xff7070e0, // String
	    0xff70a0e0, // Char literal
	    0xffffffff, // Punctuation
	    0xff408080, // Preprocessor
	    0xffaaaaaa, // Identifier
	    0xff9bc64d, // Known identifier
	    0xffc040a0, // Preproc identifier
	    0xff206020, // Comment (single line)
	    0xff406020, // Comment (multi line)
	    0xff101010, // Background
	    0xffe0e0e0, // Cursor
	    0x80a06020, // Selection
	    0x800020ff, // ErrorMarker
	    0x40f08000, // Breakpoint
	    0xff707000, // Line number
	    0x40000000, // Current line fill
	    0x40808080, // Current line fill (inactive)
	    0x40a0a0a0, // Current line edge
	}));

	return true;
}

bool EditorStyles::Majorelle()
{
	ImVec4* colors = ImGui::GetStyle().Colors;

	colors[ImGuiCol_Text] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.98f);
	colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.30f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 1.00f, 1.00f, 0.51f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.98f, 0.98f, 0.98f, 0.53f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.69f, 0.69f, 0.69f, 0.80f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.49f, 0.49f, 0.49f, 0.80f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.46f, 0.54f, 0.80f, 0.60f);
	colors[ImGuiCol_Button] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_Separator] = ImVec4(0.39f, 0.39f, 0.39f, 0.62f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.14f, 0.44f, 0.80f, 0.78f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.14f, 0.44f, 0.80f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.80f, 0.80f, 0.80f, 0.56f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	colors[ImGuiCol_Tab] = ImLerp(colors[ImGuiCol_Header], colors[ImGuiCol_TitleBgActive], 0.90f);
	colors[ImGuiCol_TabHovered] = colors[ImGuiCol_HeaderHovered];
	colors[ImGuiCol_TabActive] = ImLerp(colors[ImGuiCol_HeaderActive], colors[ImGuiCol_TitleBgActive], 0.60f);
	colors[ImGuiCol_TabUnfocused] = ImLerp(colors[ImGuiCol_Tab], colors[ImGuiCol_TitleBg], 0.80f);
	colors[ImGuiCol_TabUnfocusedActive] = ImLerp(colors[ImGuiCol_TabActive], colors[ImGuiCol_TitleBg], 0.40f);
	colors[ImGuiCol_DockingPreview] = colors[ImGuiCol_Header] * ImVec4(1.0f, 1.0f, 1.0f, 0.7f);
	colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.45f, 0.00f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	colors[ImGuiCol_NavHighlight] = colors[ImGuiCol_HeaderHovered];
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.70f, 0.70f, 0.70f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);

	TextEditor::SetPalette(TextEditor::Palette({
	    0xff000000, // None
	    0xffff0c06, // Keyword
	    0xff008000, // Number
	    0xff2020a0, // String
	    0xff304070, // Char literal
	    0xff000000, // Punctuation
	    0xff406060, // Preprocessor
	    0xff404040, // Identifier
	    0xff606010, // Known identifier
	    0xffc040a0, // Preproc identifier
	    0xff205720, // Comment (single line)
	    0xff405720, // Comment (multi line)
	    0xffffffff, // Background
	    0xff000000, // Cursor
	    0x80600000, // Selection
	    0xa00010ff, // ErrorMarker
	    0x80f08000, // Breakpoint
	    0xff505000, // Line number
	    0x40000000, // Current line fill
	    0x40808080, // Current line fill (inactive)
	    0x40000000, // Current line edge
	}));

	return false;
}

bool EditorStyles::Cherry()
{
	ImVec4* colors = ImGui::GetStyle().Colors;

	colors[ImGuiCol_Text] = ImVec4(0.860f, 0.930f, 0.890f, 0.78f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.860f, 0.930f, 0.890f, 0.28f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.14f, 0.17f, 1.00f);
	colors[ImGuiCol_Tab] = ImVec4(0.200f, 0.220f, 0.270f, 0.58f);
	colors[ImGuiCol_TabActive] = ImVec4(0.200f, 0.220f, 0.270f, 0.58f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.232f, 0.201f, 0.271f, 1.00f);
	colors[ImGuiCol_TabUnfocused] = ImLerp(colors[ImGuiCol_Tab], colors[ImGuiCol_TitleBg], 0.80f);
	colors[ImGuiCol_TabUnfocusedActive] = ImLerp(colors[ImGuiCol_TabActive], colors[ImGuiCol_TitleBg], 0.40f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.200f, 0.220f, 0.270f, 0.9f);
	colors[ImGuiCol_Border] = ImVec4(0.31f, 0.31f, 1.00f, 0.00f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.200f, 0.220f, 0.270f, 1.00f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.455f, 0.198f, 0.301f, 0.78f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.455f, 0.198f, 0.301f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.502f, 0.075f, 0.256f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.200f, 0.220f, 0.270f, 0.75f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.200f, 0.220f, 0.270f, 0.47f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.200f, 0.220f, 0.270f, 1.00f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.09f, 0.15f, 0.16f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.455f, 0.198f, 0.301f, 0.78f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.455f, 0.198f, 0.301f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.71f, 0.22f, 0.27f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.47f, 0.77f, 0.83f, 0.14f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.71f, 0.22f, 0.27f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(0.47f, 0.77f, 0.83f, 0.14f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.455f, 0.198f, 0.301f, 0.86f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.455f, 0.198f, 0.301f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.455f, 0.198f, 0.301f, 0.76f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.455f, 0.198f, 0.301f, 0.86f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.502f, 0.075f, 0.256f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.47f, 0.77f, 0.83f, 0.04f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.455f, 0.198f, 0.301f, 0.78f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.455f, 0.198f, 0.301f, 1.00f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.860f, 0.930f, 0.890f, 0.63f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.455f, 0.198f, 0.301f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.860f, 0.930f, 0.890f, 0.63f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.455f, 0.198f, 0.301f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.455f, 0.198f, 0.301f, 0.43f);
	colors[ImGuiCol_TabHovered] = colors[ImGuiCol_HeaderHovered];
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.200f, 0.220f, 0.270f, 0.73f);
	colors[ImGuiCol_DockingPreview] = colors[ImGuiCol_HeaderActive] * ImVec4(1.0f, 1.0f, 1.0f, 0.7f);
	colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.455f, 0.198f, 0.301f, 0.78f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.455f, 0.198f, 0.301f, 0.69f);

	TextEditor::SetPalette(TextEditor::Palette({
	    0xffffffff, // Default
	    0xffd69c56, // Keyword
	    0xff00df00, // Number
	    0xff7070e6, // String
	    0xff70a0e6, // Char literal
	    0xffffffff, // Punctuation
	    0xff408080, // Preprocessor
	    0xffaaaaaa, // Identifier
	    0xff9bc64d, // Known identifier
	    0xffc040a0, // Preproc identifier
	    0xff206020, // Comment (single line)
	    0xff406020, // Comment (multi line)
	    0xff101010, // Background
	    0xffe0e0e0, // Cursor
	    0x80a06020, // Selection
	    0x800020ff, // ErrorMarker
	    0x40f08000, // Breakpoint
	    0xff707000, // Line number
	    0x40000000, // Current line fill
	    0x40808080, // Current line fill (inactive)
	    0x40a0a0a0, // Current line edge
	}));

	return true;
}

bool EditorStyles::Xiketic()
{
	ImVec4* colors = ImGui::GetStyle().Colors;

	colors[ImGuiCol_Text] = ImVec4(0.90f, 0.90f, 0.90f, 0.90f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.09f, 0.09f, 0.15f, 1.00f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.05f, 0.05f, 0.10f, 0.85f);
	colors[ImGuiCol_Border] = ImVec4(0.70f, 0.70f, 0.70f, 0.65f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.00f, 0.00f, 0.01f, 1.00f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.90f, 0.80f, 0.80f, 0.40f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.90f, 0.65f, 0.65f, 0.45f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.83f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.40f, 0.40f, 0.80f, 0.20f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.00f, 0.00f, 0.00f, 0.87f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.01f, 0.01f, 0.02f, 0.80f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.20f, 0.25f, 0.30f, 0.60f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.55f, 0.53f, 0.55f, 0.51f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.56f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.91f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.90f, 0.90f, 0.90f, 0.83f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.70f, 0.70f, 0.70f, 0.62f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.30f, 0.30f, 0.30f, 0.84f);
	colors[ImGuiCol_Button] = ImVec4(0.48f, 0.72f, 0.89f, 0.49f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.50f, 0.69f, 0.99f, 0.68f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.80f, 0.50f, 0.50f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.30f, 0.69f, 1.00f, 0.53f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.44f, 0.61f, 0.86f, 1.00f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.38f, 0.62f, 0.83f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.85f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.60f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(1.00f, 1.00f, 1.00f, 0.90f);
	colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.00f, 0.00f, 1.00f, 0.35f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
	colors[ImGuiCol_Tab] = ImLerp(colors[ImGuiCol_Header], colors[ImGuiCol_TitleBgActive], 0.80f);
	colors[ImGuiCol_TabHovered] = colors[ImGuiCol_HeaderHovered];
	colors[ImGuiCol_TabActive] = ImLerp(colors[ImGuiCol_HeaderActive], colors[ImGuiCol_TitleBgActive], 0.60f);
	colors[ImGuiCol_TabUnfocused] = ImLerp(colors[ImGuiCol_Tab], colors[ImGuiCol_TitleBg], 0.80f);
	colors[ImGuiCol_TabUnfocusedActive] = ImLerp(colors[ImGuiCol_TabActive], colors[ImGuiCol_TitleBg], 0.40f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.75f, 0.75f, 0.75f, 0.78f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.65f, 0.65f, 0.65f, 1.00f);
	colors[ImGuiCol_DockingPreview] = colors[ImGuiCol_HeaderActive] * ImVec4(1.0f, 1.0f, 1.0f, 0.7f);
	colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);

	TextEditor::SetPalette(TextEditor::Palette({
	    0xff7f7f7f, // Default
	    0xffd69c56, // Keyword
	    0xff00df00, // Number
	    0xff7070e0, // String
	    0xff70a0e0, // Char literal
	    0xffffffff, // Punctuation
	    0xff408080, // Preprocessor
	    0xffaaaaaa, // Identifier
	    0xff9bc64d, // Known identifier
	    0xffc040a0, // Preproc identifier
	    0xff206020, // Comment (single line)
	    0xff406020, // Comment (multi line)
	    0xff101010, // Background
	    0xffe0e0e0, // Cursor
	    0x80a06020, // Selection
	    0x800020ff, // ErrorMarker
	    0x40f08000, // Breakpoint
	    0xff707000, // Line number
	    0x40000000, // Current line fill
	    0x40808080, // Current line fill (inactive)
	    0x40a0a0a0, // Current line edge
	}));

	return true;
}

bool EditorStyles::Onyx()
{
	ImVec4* colors = ImGui::GetStyle().Colors;

	colors[ImGuiCol_Text] = {0.73333335f, 0.73333335f, 0.73333335f, 1.00f};
	colors[ImGuiCol_TextDisabled] = {0.34509805f, 0.34509805f, 0.34509805f, 1.00f};
	colors[ImGuiCol_WindowBg] = {0.23529413f, 0.24705884f, 0.25490198f, 0.94f};
	colors[ImGuiCol_ChildBg] = {0.23529413f, 0.24705884f, 0.25490198f, 0.00f};
	colors[ImGuiCol_PopupBg] = {0.23529413f, 0.24705884f, 0.25490198f, 0.94f};
	colors[ImGuiCol_Border] = {0.33333334f, 0.33333334f, 0.33333334f, 0.50f};
	colors[ImGuiCol_BorderShadow] = {0.15686275f, 0.15686275f, 0.15686275f, 0.00f};
	colors[ImGuiCol_FrameBg] = {0.16862746f, 0.16862746f, 0.16862746f, 0.54f};
	colors[ImGuiCol_FrameBgHovered] = {0.453125f, 0.67578125f, 0.99609375f, 0.67f};
	colors[ImGuiCol_FrameBgActive] = {0.47058827f, 0.47058827f, 0.47058827f, 0.67f};
	colors[ImGuiCol_TitleBg] = {0.04f, 0.04f, 0.04f, 1.00f};
	colors[ImGuiCol_TitleBgCollapsed] = {0.16f, 0.29f, 0.48f, 1.00f};
	colors[ImGuiCol_TitleBgActive] = {0.00f, 0.00f, 0.00f, 0.51f};
	colors[ImGuiCol_MenuBarBg] = {0.27058825f, 0.28627452f, 0.2901961f, 0.80f};
	colors[ImGuiCol_ScrollbarBg] = {0.27058825f, 0.28627452f, 0.2901961f, 0.60f};
	colors[ImGuiCol_ScrollbarGrab] = {0.21960786f, 0.30980393f, 0.41960788f, 0.51f};
	colors[ImGuiCol_ScrollbarGrabHovered] = {0.21960786f, 0.30980393f, 0.41960788f, 1.00f};
	colors[ImGuiCol_ScrollbarGrabActive] = {0.13725491f, 0.19215688f, 0.2627451f, 0.91f};
	colors[ImGuiCol_CheckMark] = {0.90f, 0.90f, 0.90f, 0.83f};
	colors[ImGuiCol_SliderGrab] = {0.70f, 0.70f, 0.70f, 0.62f};
	colors[ImGuiCol_SliderGrabActive] = {0.30f, 0.30f, 0.30f, 0.84f};
	colors[ImGuiCol_Button] = {0.33333334f, 0.3529412f, 0.36078432f, 0.49f};
	colors[ImGuiCol_ButtonHovered] = {0.21960786f, 0.30980393f, 0.41960788f, 1.00f};
	colors[ImGuiCol_ButtonActive] = {0.13725491f, 0.19215688f, 0.2627451f, 1.00f};
	colors[ImGuiCol_Header] = {0.33333334f, 0.3529412f, 0.36078432f, 0.53f};
	colors[ImGuiCol_HeaderHovered] = {0.453125f, 0.67578125f, 0.99609375f, 0.67f};
	colors[ImGuiCol_HeaderActive] = {0.47058827f, 0.47058827f, 0.47058827f, 0.67f};
	colors[ImGuiCol_Separator] = {0.31640625f, 0.31640625f, 0.31640625f, 1.00f};
	colors[ImGuiCol_SeparatorHovered] = {0.31640625f, 0.31640625f, 0.31640625f, 1.00f};
	colors[ImGuiCol_SeparatorActive] = {0.31640625f, 0.31640625f, 0.31640625f, 1.00f};
	colors[ImGuiCol_ResizeGrip] = {1.00f, 1.00f, 1.00f, 0.85f};
	colors[ImGuiCol_ResizeGripHovered] = {1.00f, 1.00f, 1.00f, 0.60f};
	colors[ImGuiCol_ResizeGripActive] = {1.00f, 1.00f, 1.00f, 0.90f};
	colors[ImGuiCol_PlotLines] = {0.61f, 0.61f, 0.61f, 1.00f};
	colors[ImGuiCol_PlotLinesHovered] = {1.00f, 0.43f, 0.35f, 1.00f};
	colors[ImGuiCol_PlotHistogram] = {0.90f, 0.70f, 0.00f, 1.00f};
	colors[ImGuiCol_PlotHistogramHovered] = {1.00f, 0.60f, 0.00f, 1.00f};
	colors[ImGuiCol_TextSelectedBg] = {0.18431373f, 0.39607847f, 0.79215693f, 0.90f};
	colors[ImGuiCol_Tab] = ImLerp(colors[ImGuiCol_Header], colors[ImGuiCol_TitleBgActive], 0.80f);
	colors[ImGuiCol_TabHovered] = colors[ImGuiCol_HeaderHovered];
	colors[ImGuiCol_TabActive] = ImLerp(colors[ImGuiCol_HeaderActive], colors[ImGuiCol_TitleBgActive], 0.60f);
	colors[ImGuiCol_TabUnfocused] = ImLerp(colors[ImGuiCol_Tab], colors[ImGuiCol_TitleBg], 0.80f);
	colors[ImGuiCol_TabUnfocusedActive] = ImLerp(colors[ImGuiCol_TabActive], colors[ImGuiCol_TitleBg], 0.40f);
	colors[ImGuiCol_DockingPreview] = colors[ImGuiCol_HeaderActive] * ImVec4(1.0f, 1.0f, 1.0f, 0.7f);
	colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);

	TextEditor::SetPalette(TextEditor::Palette({
	    0xffdfdfdf, // Default
	    0xffdf9c56, // Keyword
	    0xff00df00, // Number
	    0xff7070e0, // String
	    0xff70a0e0, // Char literal
	    0xffffffff, // Punctuation
	    0xff408080, // Preprocessor
	    0xffaaaaaa, // Identifier
	    0xff9bc64d, // Known identifier
	    0xffc040a0, // Preproc identifier
	    0xff207020, // Comment (single line)
	    0xff407020, // Comment (multi line)
	    0xff101010, // Background
	    0xffe0e0e0, // Cursor
	    0x80a06020, // Selection
	    0x800020ff, // ErrorMarker
	    0x40f08000, // Breakpoint
	    0xff707000, // Line number
	    0x40000000, // Current line fill
	    0x40808080, // Current line fill (inactive)
	    0x40a0a0a0, // Current line edge
	}));

	return true;
}

bool EditorStyles::Mono()
{
	ImVec4* colors = ImGui::GetStyle().Colors;

	colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);
	colors[ImGuiCol_ChildBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
	colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.21f, 0.22f, 0.54f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.40f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.18f, 0.18f, 0.18f, 0.67f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.29f, 0.29f, 0.29f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(0.44f, 0.44f, 0.44f, 0.40f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.46f, 0.47f, 0.48f, 1.00f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.42f, 0.42f, 0.42f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.70f, 0.70f, 0.70f, 0.31f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.70f, 0.70f, 0.70f, 0.80f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.48f, 0.50f, 0.52f, 1.00f);
	colors[ImGuiCol_Separator] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.72f, 0.72f, 0.72f, 0.78f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.91f, 0.91f, 0.91f, 0.25f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.81f, 0.81f, 0.81f, 0.67f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.46f, 0.46f, 0.46f, 0.95f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.73f, 0.60f, 0.15f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.87f, 0.87f, 0.87f, 0.35f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	colors[ImGuiCol_NavHighlight] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_Tab] = ImLerp(colors[ImGuiCol_Header], colors[ImGuiCol_TitleBgActive], 0.80f);
	colors[ImGuiCol_TabHovered] = colors[ImGuiCol_HeaderHovered];
	colors[ImGuiCol_TabActive] = ImLerp(colors[ImGuiCol_HeaderActive], colors[ImGuiCol_TitleBgActive], 0.60f);
	colors[ImGuiCol_TabUnfocused] = ImLerp(colors[ImGuiCol_Tab], colors[ImGuiCol_TitleBg], 0.80f);
	colors[ImGuiCol_TabUnfocusedActive] = ImLerp(colors[ImGuiCol_TabActive], colors[ImGuiCol_TitleBg], 0.40f);
	colors[ImGuiCol_DockingPreview] = colors[ImGuiCol_HeaderActive] * ImVec4(1.0f, 1.0f, 1.0f, 0.7f);
	colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);

	TextEditor::SetPalette(TextEditor::Palette({
	    0xff7f7f7f, // Default
	    0xffd69c56, // Keyword
	    0xff00df00, // Number
	    0xff7070e5, // String
	    0xff70a0e5, // Char literal
	    0xffffffff, // Punctuation
	    0xff408080, // Preprocessor
	    0xffaaaaaa, // Identifier
	    0xff9bc64d, // Known identifier
	    0xffc040a0, // Preproc identifier
	    0xff206020, // Comment (single line)
	    0xff406020, // Comment (multi line)
	    0xff101010, // Background
	    0xffe0e0e0, // Cursor
	    0x80a06020, // Selection
	    0x800020ff, // ErrorMarker
	    0x40f08000, // Breakpoint
	    0xff707000, // Line number
	    0x40000000, // Current line fill
	    0x40808080, // Current line fill (inactive)
	    0x40a0a0a0, // Current line edge
	}));

	return true;
}

bool EditorStyles::Emerald()
{
	ImVec4* colors = ImGui::GetStyle().Colors;

	colors[ImGuiCol_Text] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.93f, 0.93f, 0.93f, 0.98f);
	colors[ImGuiCol_Border] = ImVec4(0.71f, 0.71f, 0.71f, 0.08f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.04f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.71f, 0.71f, 0.71f, 0.55f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.94f, 0.94f, 0.94f, 0.55f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.71f, 0.78f, 0.69f, 0.98f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.85f, 0.85f, 0.85f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.82f, 0.78f, 0.78f, 0.51f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.78f, 0.78f, 0.78f, 1.00f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.20f, 0.25f, 0.30f, 0.61f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.90f, 0.90f, 0.90f, 0.30f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.92f, 0.92f, 0.92f, 0.78f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.184f, 0.407f, 0.193f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.06f, 0.78f, 0.39f, 0.78f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.16f, 0.88f, 0.49f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(0.71f, 0.78f, 0.69f, 0.40f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.725f, 0.805f, 0.702f, 1.00f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.793f, 0.900f, 0.836f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.71f, 0.78f, 0.69f, 0.31f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.71f, 0.78f, 0.69f, 0.80f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.71f, 0.78f, 0.69f, 1.00f);
	colors[ImGuiCol_Separator] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.14f, 0.80f, 0.44f, 0.78f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.14f, 0.80f, 0.44f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.79f, 0.38f, 0.45f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.79f, 0.38f, 0.78f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	colors[ImGuiCol_NavHighlight] = colors[ImGuiCol_HeaderHovered];
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.70f, 0.70f, 0.70f, 0.70f);
	colors[ImGuiCol_Tab] = ImLerp(colors[ImGuiCol_Header], colors[ImGuiCol_TitleBgActive], 0.80f);
	colors[ImGuiCol_TabHovered] = colors[ImGuiCol_HeaderHovered];
	colors[ImGuiCol_TabActive] = ImLerp(colors[ImGuiCol_HeaderActive], colors[ImGuiCol_TitleBgActive], 0.60f);
	colors[ImGuiCol_TabUnfocused] = ImLerp(colors[ImGuiCol_Tab], colors[ImGuiCol_TitleBg], 0.80f);
	colors[ImGuiCol_TabUnfocusedActive] = ImLerp(colors[ImGuiCol_TabActive], colors[ImGuiCol_TitleBg], 0.40f);
	colors[ImGuiCol_DockingPreview] = colors[ImGuiCol_HeaderActive] * ImVec4(1.0f, 1.0f, 1.0f, 0.7f);
	colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);

	TextEditor::SetPalette(TextEditor::Palette({
	    0xff000000, // None
	    0xffff0c06, // Keyword
	    0xff008000, // Number
	    0xff2020a0, // String
	    0xff304070, // Char literal
	    0xff000000, // Punctuation
	    0xff406060, // Preprocessor
	    0xff404040, // Identifier
	    0xff606010, // Known identifier
	    0xffc040a0, // Preproc identifier
	    0xff205f20, // Comment (single line)
	    0xff405f20, // Comment (multi line)
	    0xffffffff, // Background
	    0xff000000, // Cursor
	    0x80600000, // Selection
	    0xa00010ff, // ErrorMarker
	    0x80f08000, // Breakpoint
	    0xff505000, // Line number
	    0x40000000, // Current line fill
	    0x40808080, // Current line fill (inactive)
	    0x40000000, // Current line edge
	}));

	return false;
}

bool EditorStyles::Ochre()
{
	ImVec4* colors = ImGui::GetStyle().Colors;

	colors[ImGuiCol_Text] = ImVec4(0.92f, 0.92f, 0.92f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.44f, 0.44f, 0.44f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
	colors[ImGuiCol_Border] = ImVec4(0.51f, 0.36f, 0.15f, 1.00f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.51f, 0.36f, 0.15f, 1.00f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.78f, 0.55f, 0.21f, 1.00f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.51f, 0.36f, 0.15f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.91f, 0.64f, 0.13f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.06f, 0.06f, 0.06f, 0.53f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.21f, 0.21f, 0.21f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.47f, 0.47f, 0.47f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.81f, 0.83f, 0.81f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.78f, 0.55f, 0.21f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.91f, 0.64f, 0.13f, 1.00f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.91f, 0.64f, 0.13f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(0.51f, 0.36f, 0.15f, 1.00f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.91f, 0.64f, 0.13f, 1.00f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.78f, 0.55f, 0.21f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.51f, 0.36f, 0.15f, 1.00f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.91f, 0.64f, 0.13f, 1.00f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.93f, 0.65f, 0.14f, 1.00f);
	colors[ImGuiCol_Separator] = ImVec4(0.21f, 0.21f, 0.21f, 1.00f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.91f, 0.64f, 0.13f, 1.00f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.78f, 0.55f, 0.21f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.21f, 0.21f, 0.21f, 1.00f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.91f, 0.64f, 0.13f, 1.00f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.78f, 0.55f, 0.21f, 1.00f);
	colors[ImGuiCol_Tab] = ImVec4(0.51f, 0.36f, 0.15f, 1.00f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.91f, 0.64f, 0.13f, 1.00f);
	colors[ImGuiCol_TabActive] = ImVec4(0.78f, 0.55f, 0.21f, 1.00f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.07f, 0.10f, 0.15f, 0.97f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.42f, 0.26f, 0.14f, 1.00f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
	colors[ImGuiCol_DockingPreview] = colors[ImGuiCol_HeaderActive] * ImVec4(1.0f, 1.0f, 1.0f, 0.7f);
	colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);

	TextEditor::SetPalette(TextEditor::Palette({
	    0xff7f7f7f, // Default
	    0xffd69c56, // Keyword
	    0xff00ff00, // Number
	    0xff7070e0, // String
	    0xff70a0e0, // Char literal
	    0xffffffff, // Punctuation
	    0xff408080, // Preprocessor
	    0xffaaaaaa, // Identifier
	    0xff9bc64d, // Known identifier
	    0xffc040a0, // Preproc identifier
	    0xff206020, // Comment (single line)
	    0xff406020, // Comment (multi line)
	    0xff101010, // Background
	    0xffe0e0e0, // Cursor
	    0x80a06020, // Selection
	    0x800020ff, // ErrorMarker
	    0x40f08000, // Breakpoint
	    0xff707000, // Line number
	    0x40000000, // Current line fill
	    0x40808080, // Current line fill (inactive)
	    0x40a0a0a0, // Current line edge
	}));

	return true;
}
