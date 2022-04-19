#include "stdafx.h"
#include "ConsolePanel.h"

#include "ImGui/ImGuiUtilites.h"
#include "imgui/imgui.h"
#include "Fonts/Fonts.h"
#include "IconsFontAwesome5.h"

bool ConsolePanel::s_DarkTheme = false;

void ConsolePanel::Clear()
{
	InternalConsole::s_MessageBuffer.clear();
	InternalConsole::s_MessageBuffer.resize(InternalConsole::s_MessageBufferCapacity);

	InternalConsole::s_MessageBufferBegin = 0;
}

void ConsolePanel::SetDarkTheme(bool darktheme)
{
	s_DarkTheme = darktheme;
}

void ConsolePanel::OnImGuiRender()
{
	if (*m_Show)
	{
		ImGui::SetNextWindowSize(ImVec2(640, 480), ImGuiCond_FirstUseEver);
		ImGui::Begin(ICON_FA_TERMINAL" Console", m_Show);
		{
			ImGuiRenderHeader();
			ImGui::Separator();
			ImGui::PushFont(Fonts::Consolas);
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 1.0f));
			ImGuiRenderMessages();
			ImGui::PopFont();
			ImGui::PopStyleVar();
		}
		ImGui::End();
	}
}

ConsolePanel::ConsolePanel(bool* show)
	:Layer("Console"), m_Show(show)
{
	m_DisplayScale = 1.0f;

	m_LevelFilter = Level::Trace;
	m_AllowScrollingToBottom = true;

	m_LastBufferSize = 0;

	m_TextFilter = new ImGuiTextFilter();
}

void ConsolePanel::ImGuiRenderHeader()
{
	ImGuiStyle& style = ImGui::GetStyle();
	const float spacing = style.ItemInnerSpacing.x;

	ImGui::SameLine(0.0f, 2.0f * spacing);

	// Button to quickly change level left
	if (ImGui::ArrowButton("##MessageRenderFilter_L", ImGuiDir_Left))
	{
		m_LevelFilter = GetLowerLevel(m_LevelFilter);
	}

	ImGui::Tooltip("Lower Level");

	ImGui::SameLine(0.0f, spacing);

	// Drop-down with Levels
	ImGui::PushItemWidth(ImGui::CalcTextSize("Critical").x * 1.36f);
	if (ImGui::BeginCombo(
		"##MessageRenderFilter",
		GetLevelName(m_LevelFilter),
		ImGuiComboFlags_NoArrowButton))
	{
		for (auto level = s_Levels.begin(); level != s_Levels.end(); level++)
		{
			bool is_selected = (m_LevelFilter == *level);
			if (ImGui::Selectable(GetLevelName(*level), is_selected))
				m_LevelFilter = *level;
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
	ImGui::PopItemWidth();

	ImGui::Tooltip("Level");

	ImGui::SameLine(0.0f, spacing);

	// Buttons to quickly change level right
	if (ImGui::ArrowButton("##MessageRenderFilter_R", ImGuiDir_Right))
	{
		m_LevelFilter = GetHigherLevel(m_LevelFilter);
	}

	ImGui::Tooltip("Higher Level");

	//Filter text box
	ImGui::SameLine(0.0f, spacing);
	m_TextFilter->Draw("Search", 180);

	ImGui::Tooltip("Filter (\"incl,-excl\")");

	ImGui::SameLine(0.0f, spacing);

	// Button for advanced Settings
	ImGui::SameLine(0.0f, ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize("Settings").x - style.WindowPadding.x / 2.0f);
	if (ImGui::Button("Settings"))
		ImGui::OpenPopup("SettingsPopup");
	if (ImGui::BeginPopup("SettingsPopup"))
	{
		ImGuiRenderSettings();
		ImGui::EndPopup();
	}
}

void ConsolePanel::ImGuiRenderSettings()
{
	const float maxWidth = ImGui::CalcTextSize("Scroll to bottom").x * 1.1f;
	const float spacing = ImGui::GetStyle().ItemInnerSpacing.x + ImGui::CalcTextSize(" ").x;

	// checkbox for scrolling lock
	ImGui::Text("Scroll to bottom");
	ImGui::SameLine(0.0f, spacing + maxWidth - ImGui::CalcTextSize("Scroll to bottom").x);
	ImGui::Checkbox("##ScrollToBottom", &m_AllowScrollingToBottom);

	ImGui::SameLine(0.0f, spacing);

	// Button to clear the console
	ImGui::SameLine();
	if (ImGui::Button("Clear Console"))
		Clear();

	// Slider for font scale
	ImGui::Text("Display scale");
	ImGui::SameLine(0.0f, spacing + maxWidth - ImGui::CalcTextSize("Display Scale").x);
	ImGui::PushItemWidth(maxWidth * 1.25f / 1.1f);
	ImGui::SliderFloat("##DisplayScale", &m_DisplayScale, 0.5f, 4.0f, "%.1f");
	ImGui::PopItemWidth();
}

void ConsolePanel::ImGuiRenderMessages()
{
	ImGui::BeginChild("ScrollRegion", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
	{
		ImGui::SetWindowFontScale(m_DisplayScale);

		auto messageStart = InternalConsole::s_MessageBuffer.begin() + InternalConsole::s_MessageBufferBegin;
		if (InternalConsole::s_MessageBufferBegin == 0)// If contains old message here
			for (auto message : InternalConsole::s_MessageBuffer)
				RenderMessage(message);
		if (InternalConsole::s_MessageBufferBegin != 0) // Skipped first message in vector
			for (auto message = InternalConsole::s_MessageBuffer.begin(); message != messageStart; message++)
				RenderMessage(*message);

		if (m_AllowScrollingToBottom && m_LastBufferSize < InternalConsole::s_MessageBufferSize && ImGui::GetScrollMaxY() > 0)
		{
			ImGui::SetScrollY(ImGui::GetScrollMaxY());
			m_LastBufferSize = InternalConsole::s_MessageBufferSize;
		}
	}
	ImGui::EndChild();
}

std::vector<ConsolePanel::Level> ConsolePanel::s_Levels
{
	ConsolePanel::Level::Trace,
	ConsolePanel::Level::Debug,
	ConsolePanel::Level::Info,
	ConsolePanel::Level::Warn,
	ConsolePanel::Level::Error,
	ConsolePanel::Level::Critical,
	ConsolePanel::Level::Off
};

void ConsolePanel::RenderMessage(const InternalConsole::Message& message)
{
	Level level = GetMessageLevel(message.second);
	if (level != Level::Invalid && level >= m_LevelFilter && m_TextFilter->PassFilter(message.first.c_str()))
	{
		Colour colour = GetRenderColour(level);
		ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + ImGui::GetContentRegionAvail().x);
		ImGui::TextColored({ colour.r, colour.g, colour.b, colour.a }, "%s", message.first.c_str());
		ImGui::PopTextWrapPos();
	}
}

ConsolePanel::Level ConsolePanel::GetMessageLevel(spdlog::level::level_enum level)
{
	switch (level)
	{
	case spdlog::level::level_enum::trace: return ConsolePanel::Level::Trace;
	case spdlog::level::level_enum::debug:return ConsolePanel::Level::Debug;
	case spdlog::level::level_enum::info:return ConsolePanel::Level::Info;
	case spdlog::level::level_enum::warn:return ConsolePanel::Level::Warn;
	case spdlog::level::level_enum::err:return ConsolePanel::Level::Error;
	case spdlog::level::level_enum::critical:return ConsolePanel::Level::Critical;
	case spdlog::level::level_enum::off:return ConsolePanel::Level::Off;
	default:
		break;
	}

	return ConsolePanel::Level::Invalid;
}


ConsolePanel::Level ConsolePanel::GetLowerLevel(Level level)
{
	switch (level)
	{
	case ConsolePanel::Level::Off:			return ConsolePanel::Level::Critical;
	case ConsolePanel::Level::Critical:	return ConsolePanel::Level::Error;
	case ConsolePanel::Level::Error:		return ConsolePanel::Level::Warn;
	case ConsolePanel::Level::Warn:		//return ConsolePanel::Level::Debug;
	case ConsolePanel::Level::Debug:		return ConsolePanel::Level::Info;
	case ConsolePanel::Level::Info:
	case ConsolePanel::Level::Trace:		return ConsolePanel::Level::Trace;
	default:
		return ConsolePanel::Level::Invalid;
	}
}

ConsolePanel::Level ConsolePanel::GetHigherLevel(Level level)
{
	switch (level)
	{
	case ConsolePanel::Level::Trace:		return ConsolePanel::Level::Info;
	case ConsolePanel::Level::Info:		//return ConsolePanel::Level::Debug;
	case ConsolePanel::Level::Debug:		return ConsolePanel::Level::Warn;
	case ConsolePanel::Level::Warn:
	case ConsolePanel::Level::Error:		return ConsolePanel::Level::Error;
	case ConsolePanel::Level::Critical:	return ConsolePanel::Level::Critical;
	case ConsolePanel::Level::Off:			return ConsolePanel::Level::Off;
	default:
		return ConsolePanel::Level::Invalid;
	}
}

const char* ConsolePanel::GetLevelName(Level level)
{
	switch (level)
	{
	case ConsolePanel::Level::Trace: return "Trace";
	case ConsolePanel::Level::Info: return "Info";
	case ConsolePanel::Level::Debug:return "Debug";
	case ConsolePanel::Level::Warn:return "Warn";
	case ConsolePanel::Level::Error:return "Error";
	case ConsolePanel::Level::Critical:return "Critical";
	case ConsolePanel::Level::Off: return "None";
	default:
		return "Unknown Level";
	}
}

Colour ConsolePanel::GetRenderColour(Level level)
{
	if (s_DarkTheme)
	{
		switch (level)
		{
		case ConsolePanel::Level::Trace: return Colours::SILVER;
		case ConsolePanel::Level::Info: return Colours::LIME_GREEN;
		case ConsolePanel::Level::Debug:return Colours::CYAN;
		case ConsolePanel::Level::Warn:return Colours::YELLOW;
		case ConsolePanel::Level::Error:return Colours::RED;
		case ConsolePanel::Level::Critical:return Colours::WHITE;
		default:
			return Colours::SILVER;
		}
	}
	else
	{
		switch (level)
		{
		case ConsolePanel::Level::Trace: return Colours::GREY;
		case ConsolePanel::Level::Info: return Colours::FOREST_GREEN;
		case ConsolePanel::Level::Debug:return Colours::INDIGO;
		case ConsolePanel::Level::Warn:return Colours::MUSTARD;
		case ConsolePanel::Level::Error:return Colours::RED;
		case ConsolePanel::Level::Critical:return Colours::BLACK;
		default:
			return Colours::GREY;
		}
	}
}

