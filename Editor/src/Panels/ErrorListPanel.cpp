#include "ErrorListPanel.h"
#include "IconsFontAwesome5.h"
#include "ImGui/imgui.h"

#include "Scripting/Lua/LuaManager.h"
#include "Scene/SceneManager.h"
#include "Scene/Components/Components.h"
#include "Logging/Instrumentor.h"

#include "MainDockSpace.h"
#include "Viewers/ViewerManager.h"

ErrorListPanel::ErrorListPanel(bool* show)
	: m_Show(show), Layer("ErrorList")
{
}

void ErrorListPanel::OnAttach()
{
}

void ErrorListPanel::OnUpdate(float deltaTime)
{
	m_CurrentTime += deltaTime;
	if (m_CurrentTime > m_UpdateInterval && SceneManager::GetSceneState() == SceneState::Edit && SceneManager::IsSceneLoaded())
	{
		m_ErrorList.clear();
		auto view = SceneManager::CurrentScene()->GetRegistry().view<LuaScriptComponent>();
		for (auto entity : view)
		{
			auto [luaScriptComp] = view.get(entity);

			std::optional<std::pair<int, std::string>> result = luaScriptComp.ParseScript(Entity(entity, SceneManager::CurrentScene()));
			if (result.has_value())
			{
				m_ErrorList.push_back(std::make_pair(Error(luaScriptComp.absoluteFilepath, result.value().first, result.value().second), false));
			}
		}
		m_CurrentTime = 0.0f;
		LuaManager::CleanUp();
	}
}

void ErrorListPanel::OnImGuiRender()
{
	PROFILE_FUNCTION();

	if (!*m_Show)
		return;

	ImGui::SetNextWindowSize(ImVec2(600, 600), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowPos(ImVec2(70, 50), ImGuiCond_FirstUseEver);

	if (ImGui::Begin(ICON_FA_TIMES_CIRCLE" Error List", m_Show))
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
			for(auto& [error, selected] : m_ErrorList)
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

						for (auto& [_, s] : m_ErrorList)
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
							for (auto& [e, s] : m_ErrorList)
							{
								if (s)
									ViewerManager::OpenViewer(e.filepath);
							}
						}
					}
				}

				ImGui::TableSetColumnIndex(1);
				ImGui::Text("%s", error.filepath.filename().string().c_str());
				ImGui::TableSetColumnIndex(2);
				ImGui::Text("%i",error.lineNumber);
			}
			ImGui::EndTable();
		}
	}
	ImGui::End();
}

void ErrorListPanel::AddError(Error& error)
{
	m_ErrorList.push_back(std::make_pair(error, false));
}

void ErrorListPanel::ClearAllErrors()
{
	m_ErrorList.clear();
}
