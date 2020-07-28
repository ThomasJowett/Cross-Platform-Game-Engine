#include "stdafx.h"
#include "ImGuiConsole.h"

#include "ImGuiUtilites.h"
#include "imgui.h"

uint16_t ImGuiConsole::s_MessageBufferCapacity = 200;
uint16_t ImGuiConsole::s_MessageBufferSize = 0;
uint16_t ImGuiConsole::s_MessageBufferBegin = 0;
std::vector<Ref<ImGuiConsole::Message>> ImGuiConsole::s_MessageBuffer(s_MessageBufferCapacity);

bool ImGuiConsole::Message::s_DarkTheme = false;

void ImGuiConsole::AddMessage(const std::string& message, Message::Level level)
{
	AddMessage(CreateRef<Message>(message, level));
}

void ImGuiConsole::AddMessage(Ref<Message> message)
{
	if (message->m_Level == Message::Level::Invalid)
		return;
	*(s_MessageBuffer.begin() + s_MessageBufferBegin) = message;

	if (++s_MessageBufferBegin == s_MessageBufferCapacity)
		s_MessageBufferBegin = 0;
	if (s_MessageBufferSize < s_MessageBufferCapacity)
		s_MessageBufferSize++;
}

void ImGuiConsole::Clear()
{
	for (auto message = s_MessageBuffer.begin(); message != s_MessageBuffer.end(); message++)
		(*message) = std::make_shared<Message>();

	s_MessageBufferBegin = 0;
}

void ImGuiConsole::SetDarkTheme(bool darktheme)
{
	Message::s_DarkTheme = darktheme;
}

void ImGuiConsole::OnImGuiRender()
{
	if (*m_Show)
	{
		ImGui::SetNextWindowSize(ImVec2(640, 480), ImGuiCond_FirstUseEver);
		ImGui::Begin("Console", m_Show);
		{
			ImGuiRenderHeader();
			ImGui::Separator();
			ImGuiRenderMessages();
		}
		ImGui::End();
	}
}

ImGuiConsole::ImGuiConsole(bool* show)
	:Layer("Console"), m_Show(show)
{
	m_DisplayScale = 1.0f;

	m_MessageBufferRenderFilter = Message::Level::Trace;
	m_AllowScrollingToBottom = true;

	m_LastBufferSize = 0;

	m_Filter = new ImGuiTextFilter();
}

void ImGuiConsole::ImGuiRenderHeader()
{
	ImGuiStyle& style = ImGui::GetStyle();
	const float spacing = style.ItemInnerSpacing.x;

	ImGui::SameLine(0.0f, 2.0f * spacing);

	// Button to quickly change level left
	if (ImGui::ArrowButton("##MessageRenderFilter_L", ImGuiDir_Left))
	{
		m_MessageBufferRenderFilter = Message::GetLowerLevel(m_MessageBufferRenderFilter);
	}

	ImGui::Tooltip("Lower Level");

	ImGui::SameLine(0.0f, spacing);

	// Dropdown with Levels
	ImGui::PushItemWidth(ImGui::CalcTextSize("Critical").x * 1.36f);
	if (ImGui::BeginCombo(
		"##MessageRenderFilter",
		Message::GetLevelName(m_MessageBufferRenderFilter),
		ImGuiComboFlags_NoArrowButton))
	{
		for (auto level = Message::s_Levels.begin(); level != Message::s_Levels.end(); level++)
		{
			bool is_selected = (m_MessageBufferRenderFilter == *level);
			if (ImGui::Selectable(Message::GetLevelName(*level), is_selected))
				m_MessageBufferRenderFilter = *level;
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
		m_MessageBufferRenderFilter = Message::GetHigherLevel(m_MessageBufferRenderFilter);
	}

	ImGui::Tooltip("Higher Level");

	//Filter textbox
	ImGui::SameLine(0.0f, spacing);
	m_Filter->Draw("Search", 180);

	ImGui::Tooltip("Filter (\"incl,-excl\") (\"error\")");

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

void ImGuiConsole::ImGuiRenderSettings()
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

void ImGuiConsole::ImGuiRenderMessages()
{
	ImGui::BeginChild("ScrollRegion", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
	{
		ImGui::SetWindowFontScale(m_DisplayScale);

		auto messageStart = s_MessageBuffer.begin() + s_MessageBufferBegin;
		if (*messageStart)// If contains old message here
			for (auto message = messageStart; message != s_MessageBuffer.end(); message++)
				(*message)->OnImGuiRender(m_MessageBufferRenderFilter, m_Filter);
		if (s_MessageBufferBegin != 0) // Skipped first message in vector
			for (auto message = s_MessageBuffer.begin(); message != messageStart; message++)
				(*message)->OnImGuiRender(m_MessageBufferRenderFilter, m_Filter);

		if (m_AllowScrollingToBottom && m_LastBufferSize < s_MessageBufferSize && ImGui::GetScrollMaxY() > 0)
		{
			ImGui::SetScrollY(ImGui::GetScrollMaxY());
			m_LastBufferSize = s_MessageBufferSize;
		}
	}
	ImGui::EndChild();
}

std::vector<ImGuiConsole::Message::Level> ImGuiConsole::Message::s_Levels
{
	ImGuiConsole::Message::Level::Trace,
	ImGuiConsole::Message::Level::Debug,
	ImGuiConsole::Message::Level::Info,
	ImGuiConsole::Message::Level::Warn,
	ImGuiConsole::Message::Level::Error,
	ImGuiConsole::Message::Level::Critical,
	ImGuiConsole::Message::Level::Off
};

ImGuiConsole::Message::Message(const std::string& message, Level level)
	:m_Message(message), m_Level(level)
{
}

void ImGuiConsole::Message::OnImGuiRender(Message::Level filter, ImGuiTextFilter* textFilter)
{
	if (m_Level != Level::Invalid && m_Level >= filter && textFilter->PassFilter(m_Message.c_str()))
	{
		Colour colour = GetRenderColour(m_Level);
		ImGui::TextColored({ colour.r, colour.g, colour.b, colour.a }, m_Message.c_str());
	}
}

ImGuiConsole::Message::Level ImGuiConsole::Message::GetLowerLevel(Level level)
{
	switch (level)
	{
	case ImGuiConsole::Message::Level::Off:			return ImGuiConsole::Message::Level::Critical;
	case ImGuiConsole::Message::Level::Critical:	return ImGuiConsole::Message::Level::Error;
	case ImGuiConsole::Message::Level::Error:		return ImGuiConsole::Message::Level::Warn;
	case ImGuiConsole::Message::Level::Warn:		//return ImGuiConsole::Message::Level::Debug;
	case ImGuiConsole::Message::Level::Debug:		return ImGuiConsole::Message::Level::Info;
	case ImGuiConsole::Message::Level::Info:
	case ImGuiConsole::Message::Level::Trace:		return ImGuiConsole::Message::Level::Trace;
	default:
		return ImGuiConsole::Message::Level::Invalid;
	}
}

ImGuiConsole::Message::Level ImGuiConsole::Message::GetHigherLevel(Level level)
{
	switch (level)
	{
	case ImGuiConsole::Message::Level::Trace:		return ImGuiConsole::Message::Level::Info;
	case ImGuiConsole::Message::Level::Info:		//return ImGuiConsole::Message::Level::Debug;
	case ImGuiConsole::Message::Level::Debug:		return ImGuiConsole::Message::Level::Warn;
	case ImGuiConsole::Message::Level::Warn:
	case ImGuiConsole::Message::Level::Error:		return ImGuiConsole::Message::Level::Error;
	case ImGuiConsole::Message::Level::Critical:	return ImGuiConsole::Message::Level::Critical;
	case ImGuiConsole::Message::Level::Off:			return ImGuiConsole::Message::Level::Off;
	default:
		return ImGuiConsole::Message::Level::Invalid;
	}
}

const char* ImGuiConsole::Message::GetLevelName(Level level)
{
	switch (level)
	{
	case ImGuiConsole::Message::Level::Trace: return "Trace";
	case ImGuiConsole::Message::Level::Info: return "Info";
	case ImGuiConsole::Message::Level::Debug:return "Debug";
	case ImGuiConsole::Message::Level::Warn:return "Warn";
	case ImGuiConsole::Message::Level::Error:return "Error";
	case ImGuiConsole::Message::Level::Critical:return "Critical";
	case ImGuiConsole::Message::Level::Off: return "None";
	default:
		return "Unknown Level";
	}
}

Colour ImGuiConsole::Message::GetRenderColour(Level level)
{
	if (s_DarkTheme)
	{
		switch (level)
		{
		case ImGuiConsole::Message::Level::Trace: return Colours::SILVER;
		case ImGuiConsole::Message::Level::Info: return Colours::LIME_GREEN;
		case ImGuiConsole::Message::Level::Debug:return Colours::CYAN;
		case ImGuiConsole::Message::Level::Warn:return Colours::YELLOW;
		case ImGuiConsole::Message::Level::Error:return Colours::RED;
		case ImGuiConsole::Message::Level::Critical:return Colours::WHITE;
		default:
			return Colours::SILVER;
		}
	}
	else
	{
		switch (level)
		{
		case ImGuiConsole::Message::Level::Trace: return Colours::GREY;
		case ImGuiConsole::Message::Level::Info: return Colours::FOREST_GREEN;
		case ImGuiConsole::Message::Level::Debug:return Colours::INDIGO;
		case ImGuiConsole::Message::Level::Warn:return Colours::MUSTARD;
		case ImGuiConsole::Message::Level::Error:return Colours::RED;
		case ImGuiConsole::Message::Level::Critical:return Colours::BLACK;
		default:
			return Colours::GREY;
		}
	}
}

