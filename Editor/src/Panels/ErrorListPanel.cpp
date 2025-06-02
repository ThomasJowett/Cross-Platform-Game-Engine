#include "ErrorListPanel.h"
#include "IconsFontAwesome6.h"
#include "imgui/imgui.h"

#include "Scripting/Lua/LuaManager.h"
#include "Scene/SceneManager.h"
#include "Scene/Components.h"
#include "Logging/Instrumentor.h"

#include "MainDockSpace.h"
#include "Viewers/ViewerManager.h"
#include <Events/SceneEvent.h>

ErrorListPanel::ErrorListPanel(bool* show)
	: m_Show(show), Layer("ErrorList")
{
}

void ErrorListPanel::OnAttach()
{
}

void ErrorListPanel::OnUpdate(float deltaTime)
{
}

void ErrorListPanel::OnImGuiRender()
{
	PROFILE_FUNCTION();

	if (!*m_Show)
		return;

	ImGui::SetNextWindowSize(ImVec2(600, 600), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowPos(ImVec2(70, 50), ImGuiCond_FirstUseEver);

	if (ImGui::Begin(ICON_FA_CIRCLE_XMARK" Error List", m_Show))
	{
		if (ImGui::IsWindowFocused())
		{
			MainDockSpace::SetFocussedWindow(this);
		}

		ImGuiTableFlags table_flags = 
			ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable
			| ImGuiTableFlags_Sortable | ImGuiTableFlags_SortMulti
			| ImGuiTableFlags_ScrollY | ImGuiTableFlags_NoHostExtendY;

		ImGuiSelectableFlags selectable_flags = ImGuiSelectableFlags_SpanAllColumns
			| ImGuiSelectableFlags_AllowItemOverlap
			| ImGuiSelectableFlags_AllowDoubleClick;

		if (ImGui::BeginTable("Error List", 3, table_flags))
		{
			ImGui::TableSetupColumn("Message");
			ImGui::TableSetupColumn("File");
			ImGui::TableSetupColumn("Line");
			ImGui::TableSetupScrollFreeze(0, 1);
			ImGui::TableHeadersRow();

			for (size_t i = 0; i < m_ErrorList.size(); i++)
			for(auto&& [error, selected] : m_ErrorList)
			{
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);

				if (ImGui::Selectable(m_ErrorList[i].first.message.c_str(), selected, selectable_flags))
				{
					if (!ImGui::GetIO().KeyCtrl)
					{
						if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
						{
							ViewerManager::OpenViewer(error.filepath);
						}

						for (auto&& [_, s] : m_ErrorList)
						{
							s = false;
						}
						selected = true;

						m_NumberSelected = 1;
					}
					else
					{
						m_NumberSelected -= selected;
						selected = !selected;
						m_NumberSelected += selected;

						if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
						{
							for (auto&& [e, s] : m_ErrorList)
							{
								if (s)
									ViewerManager::OpenViewer(e.filepath);
							}
						}
					}
				}

				ImGui::TableSetColumnIndex(1);
				ImGui::TextUnformatted(error.filepath.filename().string().c_str());
				ImGui::TableSetColumnIndex(2);
				ImGui::Text("%i", error.lineNumber);
			}
			ImGui::EndTable();
		}
	}
	ImGui::End();
}

void ErrorListPanel::OnEvent(Event& event)
{
	PROFILE_FUNCTION();
	EventDispatcher dispatcher(event);
	dispatcher.Dispatch<LuaErrorEvent>([this](LuaErrorEvent& luaErrorEvent)
		{
			auto error = Error(luaErrorEvent.GetFile(), luaErrorEvent.GetLine(), luaErrorEvent.GetErrorMessage());
			AddError(error);
			return false;
		});
	dispatcher.Dispatch<SceneStateChangedEvent>([this](SceneStateChangedEvent& sceneStateChangedEvent)
		{
			if (sceneStateChangedEvent.GetSceneState() == SceneState::Play
				|| sceneStateChangedEvent.GetSceneState() == SceneState::Simulate)
			{
				ClearAllErrors();
			}
			return false;
		});
}

void ErrorListPanel::AddError(Error& error)
{
	auto it = std::find_if(m_ErrorList.begin(), m_ErrorList.end(),
		[&error](const std::pair<Error, bool>& e) { return e.first.filepath == error.filepath && e.first.lineNumber == error.lineNumber; });
	if (it == m_ErrorList.end())
		m_ErrorList.push_back(std::make_pair(error, false));
}

void ErrorListPanel::ClearAllErrors()
{
	m_ErrorList.clear();
	m_NumberSelected = 0;
}
