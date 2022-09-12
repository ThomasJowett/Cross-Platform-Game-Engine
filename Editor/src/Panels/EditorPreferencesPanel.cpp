#include "EditorPreferencesPanel.h"

#include "Engine.h"

#include "imgui/imgui.h"

#include "FileSystem/FileDialog.h"

#include "Styles/EditorStyles.h"

#include "Panels/ConsolePanel.h"

#include "IconsFontAwesome5.h"

EditorPreferencesPanel::EditorPreferencesPanel(bool* show)
	:m_Show(show), Layer("Editor Preferences")
{
	m_VSnyc = Settings::GetBool("Display", "V-Sync");

	m_StyleFilename = (Application::GetWorkingDirectory() / "styles" / " Editor.style").string();
}

/* ------------------------------------------------------------------------------------------------------------------ */

void EditorPreferencesPanel::OnImGuiRender()
{
	if (!*m_Show)
	{
		return;
	}

	ImGui::SetNextWindowSize(ImVec2(640, 480), ImGuiCond_FirstUseEver);
	if (ImGui::Begin(ICON_FA_COG" Editor Preferences", m_Show, ImGuiWindowFlags_MenuBar))
	{
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Load Style"))
				{
					std::optional<std::wstring> stylePath = FileDialog::Open(L"Choose style...", L"Style Files\0*.style\0Any File\0*.*\0");
					if(stylePath)
					{
						ImGui::LoadStyle(stylePath.value(), m_Style);

						ImGuiStyle& style = ImGui::GetStyle();

						style = m_Style;
					}
				}

				if (ImGui::MenuItem("Save Style As"))
				{
					std::optional<std::wstring> stylePath = FileDialog::SaveAs(L"Save style as...", L"Style Files\0*.style\0Any File\0*.*\0");
					if(stylePath)
						ImGui::SaveStyle(stylePath.value(), m_Style);
				}
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}
		if (ImGui::TreeNode("Style"))
		{
			ShowStyleEditor();
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Display"))
		{
			if (ImGui::Checkbox("V-Sync", &m_VSnyc))
			{
				Settings::SetBool("Display", "V-Sync", m_VSnyc);
				Application::GetWindow().SetVSync(m_VSnyc);
			}
			ImGui::TreePop();
		}
	}
	ImGui::End();
}

/* ------------------------------------------------------------------------------------------------------------------ */

void EditorPreferencesPanel::OnAttach()
{
	ImGui::LoadStyle(m_StyleFilename.c_str(), m_Style);

	ImGuiStyle& style = ImGui::GetStyle();

	style = m_Style;

	ConsolePanel::SetDarkTheme(style.Colors[ImGuiCol_WindowBg].x + style.Colors[ImGuiCol_WindowBg].y + style.Colors[ImGuiCol_WindowBg].z < 1.5f);
}

/* ------------------------------------------------------------------------------------------------------------------ */

void EditorPreferencesPanel::OnDetach()
{
	ImGui::SaveStyle(m_StyleFilename.c_str(), m_Style);
}

/* ------------------------------------------------------------------------------------------------------------------ */

void EditorPreferencesPanel::ShowStyleEditor()
{
	ImGuiStyle& style = ImGui::GetStyle();

	style = m_Style;

	ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.50f);

	if (ShowStyleSelector())
		m_Style = style;

	{ bool border = (style.WindowBorderSize > 0.0f); if (ImGui::Checkbox("WindowBorder", &border)) { style.WindowBorderSize = border ? 1.0f : 0.0f; } }
	ImGui::SameLine();
	{ bool border = (style.FrameBorderSize > 0.0f);  if (ImGui::Checkbox("FrameBorder", &border)) { style.FrameBorderSize = border ? 1.0f : 0.0f; } }
	ImGui::SameLine();
	{ bool border = (style.PopupBorderSize > 0.0f);  if (ImGui::Checkbox("PopupBorder", &border)) { style.PopupBorderSize = border ? 1.0f : 0.0f; } }

	ImGui::Separator();

	if (ImGui::BeginTabBar("##tabs", ImGuiTabBarFlags_None))
	{
		if (ImGui::BeginTabItem("Sizes"))
		{
			ImGui::TextUnformatted("Main");
			ImGui::SliderFloat2("WindowPadding", (float*)&style.WindowPadding, 0.0f, 20.0f, "%.0f");
			ImGui::SliderFloat2("FramePadding", (float*)&style.FramePadding, 0.0f, 20.0f, "%.0f");
			ImGui::SliderFloat2("ItemSpacing", (float*)&style.ItemSpacing, 0.0f, 20.0f, "%.0f");
			ImGui::SliderFloat2("ItemInnerSpacing", (float*)&style.ItemInnerSpacing, 0.0f, 20.0f, "%.0f");
			ImGui::SliderFloat2("TouchExtraPadding", (float*)&style.TouchExtraPadding, 0.0f, 10.0f, "%.0f");
			ImGui::SliderFloat("IndentSpacing", &style.IndentSpacing, 0.0f, 30.0f, "%.0f");
			ImGui::SliderFloat("ScrollbarSize", &style.ScrollbarSize, 1.0f, 20.0f, "%.0f");
			ImGui::SliderFloat("GrabMinSize", &style.GrabMinSize, 1.0f, 20.0f, "%.0f");
			ImGui::TextUnformatted("Borders");
			ImGui::SliderFloat("WindowBorderSize", &style.WindowBorderSize, 0.0f, 1.0f, "%.0f");
			ImGui::SliderFloat("ChildBorderSize", &style.ChildBorderSize, 0.0f, 1.0f, "%.0f");
			ImGui::SliderFloat("PopupBorderSize", &style.PopupBorderSize, 0.0f, 1.0f, "%.0f");
			ImGui::SliderFloat("FrameBorderSize", &style.FrameBorderSize, 0.0f, 1.0f, "%.0f");
			ImGui::SliderFloat("TabBorderSize", &style.TabBorderSize, 0.0f, 1.0f, "%.0f");
			ImGui::TextUnformatted("Rounding");
			ImGui::SliderFloat("WindowRounding", &style.WindowRounding, 0.0f, 12.0f, "%.0f");
			ImGui::SliderFloat("ChildRounding", &style.ChildRounding, 0.0f, 12.0f, "%.0f");
			ImGui::SliderFloat("FrameRounding", &style.FrameRounding, 0.0f, 12.0f, "%.0f");
			ImGui::SliderFloat("PopupRounding", &style.PopupRounding, 0.0f, 12.0f, "%.0f");
			ImGui::SliderFloat("ScrollbarRounding", &style.ScrollbarRounding, 0.0f, 12.0f, "%.0f");
			ImGui::SliderFloat("GrabRounding", &style.GrabRounding, 0.0f, 12.0f, "%.0f");
			ImGui::SliderFloat("TabRounding", &style.TabRounding, 0.0f, 12.0f, "%.0f");
			ImGui::TextUnformatted("Alignment");
			ImGui::SliderFloat2("WindowTitleAlign", (float*)&style.WindowTitleAlign, 0.0f, 1.0f, "%.2f");
			int window_menu_button_position = style.WindowMenuButtonPosition + 1;
			if (ImGui::Combo("WindowMenuButtonPosition", (int*)&window_menu_button_position, "None\0Left\0Right\0"))
				style.WindowMenuButtonPosition = window_menu_button_position - 1;
			ImGui::Combo("ColorButtonPosition", (int*)&style.ColorButtonPosition, "Left\0Right\0");
			ImGui::SliderFloat2("ButtonTextAlign", (float*)&style.ButtonTextAlign, 0.0f, 1.0f, "%.2f");
			//ImGui::SameLine(); HelpMarker("Alignment applies when a button is larger than its text content.");
			ImGui::SliderFloat2("SelectableTextAlign", (float*)&style.SelectableTextAlign, 0.0f, 1.0f, "%.2f");
			//ImGui::SameLine(); HelpMarker("Alignment applies when a selectable is larger than its text content.");
			ImGui::TextUnformatted("Safe Area Padding");
			//ImGui::SameLine(); HelpMarker("Adjust if you cannot see the edges of your screen (e.g. on a TV where scaling has not been configured).");
			ImGui::SliderFloat2("DisplaySafeAreaPadding", (float*)&style.DisplaySafeAreaPadding, 0.0f, 30.0f, "%.0f");
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Colours"))
		{
			static int output_dest = 0;
			static bool output_only_modified = true;
			
			static ImGuiTextFilter filter;
			filter.Draw("Filter colours", ImGui::GetFontSize() * 16);

			static ImGuiColorEditFlags alpha_flags = 0;
			if (ImGui::RadioButton("Opaque", alpha_flags == ImGuiColorEditFlags_None)) { alpha_flags = ImGuiColorEditFlags_None; } ImGui::SameLine();
			if (ImGui::RadioButton("Alpha", alpha_flags == ImGuiColorEditFlags_AlphaPreview)) { alpha_flags = ImGuiColorEditFlags_AlphaPreview; } ImGui::SameLine();
			if (ImGui::RadioButton("Both", alpha_flags == ImGuiColorEditFlags_AlphaPreviewHalf)) { alpha_flags = ImGuiColorEditFlags_AlphaPreviewHalf; }

			ImGui::BeginChild("##colours", ImVec2(0, 0), true, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar | ImGuiWindowFlags_NavFlattened);
			ImGui::PushItemWidth(-160);
			for (int i = 0; i < ImGuiCol_COUNT; i++)
			{
				const char* name = ImGui::GetStyleColorName(i);
				if (!filter.PassFilter(name))
					continue;
				ImGui::PushID(i);
				ImGui::ColorEdit4("##colour", (float*)&style.Colors[i], ImGuiColorEditFlags_AlphaBar | alpha_flags);
				//if (memcmp(&style.Colors[i], &ref->Colours[i], sizeof(ImVec4)) != 0)
				//{
				//	// Tips: in a real user application, you may want to merge and use an icon font into the main font,
				//	// so instead of "Save"/"Revert" you'd use icons!
				//	// Read the FAQ and docs/FONTS.txt about using icon fonts. It's really easy and super convenient!
				//	ImGui::SameLine(0.0f, style.ItemInnerSpacing.x); if (ImGui::Button("Save")) { ref->Colours[i] = style.Colors[i]; }
				//	ImGui::SameLine(0.0f, style.ItemInnerSpacing.x); if (ImGui::Button("Revert")) { style.Colors[i] = ref->Colours[i]; }
				//}
				ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
				ImGui::TextUnformatted(name);
				ImGui::PopID();
			}
			ImGui::PopItemWidth();
			ImGui::EndChild();

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Fonts"))
		{
			ImGuiIO& io = ImGui::GetIO();
			ImFontAtlas* atlas = io.Fonts;
			//HelpMarker("Read FAQ and docs/FONTS.txt for details on font loading.");
			ImGui::PushItemWidth(120);
			for (int i = 0; i < atlas->Fonts.Size; i++)
			{
				ImFont* font = atlas->Fonts[i];
				ImGui::PushID(font);
				//NodeFont(font);
				ImGui::PopID();
			}
			if (ImGui::TreeNode("Atlas texture", "Atlas texture (%dx%d pixels)", atlas->TexWidth, atlas->TexHeight))
			{
				ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
				ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.5f);
				ImGui::Image(atlas->TexID, ImVec2((float)atlas->TexWidth, (float)atlas->TexHeight), ImVec2(0, 0), ImVec2(1, 1), tint_col, border_col);
				ImGui::TreePop();
			}

			// Post-baking font scaling. Note that this is NOT the nice way of scaling fonts, read below.
			// (we enforce hard clamping manually as by default DragFloat/SliderFloat allows CTRL+Click text to get out of bounds).
			const float MIN_SCALE = 0.3f;
			const float MAX_SCALE = 2.0f;
			//HelpMarker(
			//    "Those are old settings provided for convenience.\n"
			//    "However, the _correct_ way of scaling your UI is currently to reload your font at the designed size, "
			//    "rebuild the font atlas, and call style.ScaleAllSizes() on a reference ImGuiStyle structure.\n"
			//    "Using those settings here will give you poor quality results.");
			static float window_scale = 1.0f;
			if (ImGui::DragFloat("window scale", &window_scale, 0.005f, MIN_SCALE, MAX_SCALE, "%.2f"))       // Scale only this window
				ImGui::SetWindowFontScale(std::max(window_scale, MIN_SCALE));
			if (ImGui::DragFloat("global scale", &io.FontGlobalScale, 0.005f, MIN_SCALE, MAX_SCALE, "%.2f")) // Scale everything
				io.FontGlobalScale = std::max(io.FontGlobalScale, MIN_SCALE);
			ImGui::PopItemWidth();

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Rendering"))
		{
			ImGui::Checkbox("Anti-aliased lines", &style.AntiAliasedLines);
			//ImGui::SameLine(); HelpMarker("When disabling anti-aliasing lines, you'll probably want to disable borders in your style as well.");
			ImGui::Checkbox("Anti-aliased fill", &style.AntiAliasedFill);
			ImGui::PushItemWidth(100);
			ImGui::DragFloat("Curve Tessellation Tolerance", &style.CurveTessellationTol, 0.02f, 0.10f, 10.0f, "%.2f");
			if (style.CurveTessellationTol < 0.10f) style.CurveTessellationTol = 0.10f;
			ImGui::DragFloat("Circle segment Max Error", &style.CircleTessellationMaxError, 0.01f, 0.10f, 10.0f, "%.2f");
			ImGui::DragFloat("Global Alpha", &style.Alpha, 0.005f, 0.20f, 1.0f, "%.2f"); // Not exposing zero here so user doesn't "lose" the UI (zero alpha clips all widgets). But application code could have a toggle to switch between zero and non-zero.
			ImGui::PopItemWidth();

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::PopItemWidth();

	m_Style = style;
}

/* ------------------------------------------------------------------------------------------------------------------ */

bool EditorPreferencesPanel::ShowStyleSelector()
{
	static int style_idx = -1;
	if (ImGui::Combo("Colours##Selector", &style_idx, "Phthalo - Dark\0Majorelle - Light\0Cherry - Dark\0Xiketic - Dark\0Onyx - Dark\0Mono - Dark\0Emerald - Light\0Ochre - Dark"))
	{
		bool isDarkTheme = true;
		switch (style_idx)
		{
		case 0: isDarkTheme = EditorStyles::Phthalo(); break;
		case 1: isDarkTheme = EditorStyles::Majorelle(); break;
		case 2: isDarkTheme = EditorStyles::Cherry(); break;
		case 3: isDarkTheme = EditorStyles::Xiketic(); break;
		case 4: isDarkTheme = EditorStyles::Onyx(); break;
		case 5: isDarkTheme = EditorStyles::Mono(); break;
		case 6: isDarkTheme = EditorStyles::Emerald(); break;
		case 7: isDarkTheme = EditorStyles::Ochre(); break;
		}
		ConsolePanel::SetDarkTheme(isDarkTheme);
		return true;
	}
	return false;
}
