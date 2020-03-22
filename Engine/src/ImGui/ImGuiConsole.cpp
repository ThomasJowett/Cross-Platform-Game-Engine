#include "stdafx.h"
#include "ImGuiConsole.h"

#include "imgui.h"

void ImGuiConsole::AddMessage(Ref<Message> message)
{
	if (message->m_Level == Message::Level::Invalid)
		return;
	*(s_MessageBuffer.begin() + s_MessageBufferBegin) = message;

	if (++s_MessageBufferBegin == s_MessageBufferCapacity)
		s_MessageBufferBegin = 0;
	if (s_MessageBufferSize < s_MessageBufferCapacity)
		s_MessageBufferSize++;

	s_RequestScrollToBottom = s_AllowScrollingToBottom;
}

void ImGuiConsole::Clear()
{
	for (auto message = s_MessageBuffer.begin(); message != s_MessageBuffer.end(); message++)
		(*message) = std::make_shared<Message>();
	s_MessageBufferBegin = 0;
}

void ImGuiConsole::OnImGuiRender(bool* show)
{
	ImGui::SetNextWindowSize(ImVec2(640, 480), ImGuiCond_FirstUseEver);
	ImGui::Begin("Console", show);
	{
		ImGuiRendering::ImGuiRenderHeader();
		ImGui::Separator();
		ImGuiRendering::ImGuiRenderMessages();
	}
	ImGui::End();
}

void ImGuiConsole::ImGuiRendering::ImGuiRenderHeader()
{
	ImGuiStyle& style = ImGui::GetStyle();
	const float spacing = style.ItemInnerSpacing.x;

	// Text change level
	ImGui::AlignFirstTextHeightToWidgets();
	ImGui::Text("Display");

	ImGui::SameLine(0.0f, 2.0f * spacing);

	// Button to quickly change level left
	if (ImGui::ArrowButton("##MessageRenderFilter_L", ImGuiDir_Left))
	{
		s_MessageBufferRenderFilter = Message::GetLowerLevel(s_MessageBufferRenderFilter);
	}

	ImGui::SameLine(0.0f, spacing);

	// Dropdown with Levels
	ImGui::PushItemWidth(ImGui::CalcTextSize("Critical").x * 1.36f);
	if (ImGui::BeginCombo(
		"##MessageRenderFilter",
		Message::GetLevelName(s_MessageBufferRenderFilter),
		ImGuiComboFlags_NoArrowButton))
	{
		for (auto level = Message::s_Levels.begin(); level != Message::s_Levels.end(); level++)
		{
			bool is_selected = (s_MessageBufferRenderFilter == *level);
			if (ImGui::Selectable(Message::GetLevelName(*level), is_selected))
				s_MessageBufferRenderFilter = *level;
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
	ImGui::PopItemWidth();

	ImGui::SameLine(0.0f, spacing);

	// Buttons to quickly change level right
	if (ImGui::ArrowButton("##MessageRenderFilter_R", ImGuiDir_Right))
	{
		s_MessageBufferRenderFilter = Message::GetHigherLevel(s_MessageBufferRenderFilter);
	}

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
