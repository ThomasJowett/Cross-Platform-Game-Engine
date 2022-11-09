#include "VisualScriptView.h"
#include "MainDockSpace.h"
#include "IconsFontAwesome6.h"
#include "ViewerManager.h"

VisualSriptView::VisualSriptView(bool* show, const std::filesystem::path& filepath)
	:View("VisualScriptView"), m_Show(show), m_Filepath(filepath)
{
}

void VisualSriptView::OnAttach()
{
	m_LuaNodeEditor.SetFilepath(m_Filepath.string());

	m_WindowName = ICON_FA_DIAGRAM_PROJECT + std::string(" " + m_Filepath.filename().string());
}

void VisualSriptView::OnDetach()
{
	m_LuaNodeEditor.Destroy();
}

void VisualSriptView::OnImGuiRender()
{
	if (!*m_Show)
	{
		ViewerManager::CloseViewer(m_Filepath);
		return;
	}

	m_LuaNodeEditor.SetContext();

	ImGui::SetNextWindowSize(ImVec2(900, 900), ImGuiCond_FirstUseEver);

	ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoScrollWithMouse;

	if (ImGui::Begin(m_WindowName.c_str(), m_Show, flags))
	{
		if (ImGui::IsWindowFocused())
		{
			MainDockSpace::SetFocussedWindow(this);
		}

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem(ICON_FA_FLOPPY_DISK" Save", "Ctrl + S"))
					Save();
				if (ImGui::MenuItem(ICON_FA_FILE_SIGNATURE" Save As", "Ctrl + Shift + S"))
					SaveAs();
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Edit"))
			{
				if (ImGui::MenuItem(ICON_FA_ARROW_ROTATE_LEFT" Undo", "Ctrl-Z", nullptr, m_LuaNodeEditor.CanUndo()))
					m_LuaNodeEditor.Undo();
				if (ImGui::MenuItem(ICON_FA_ARROW_ROTATE_RIGHT" Redo", "Ctrl-Y", nullptr, m_LuaNodeEditor.CanRedo()))
					m_LuaNodeEditor.Redo();
				ImGui::Separator();//---------------------------------------------------------------

				if (ImGui::MenuItem(ICON_FA_SCISSORS" Cut", "Ctrl-X", nullptr, m_LuaNodeEditor.HasSelection()))
					m_LuaNodeEditor.Cut();
				if (ImGui::MenuItem(ICON_FA_COPY" Copy", "Ctrl-C", nullptr, m_LuaNodeEditor.HasSelection()))
					m_LuaNodeEditor.Copy();
				if (ImGui::MenuItem(ICON_FA_PASTE" Paste", "Ctrl-V", nullptr, ImGui::GetClipboardText() != nullptr))
					m_LuaNodeEditor.Paste();
				if (ImGui::MenuItem(ICON_FA_CLONE" Duplicate", "Ctrl-D", nullptr, m_LuaNodeEditor.HasSelection()))
					m_LuaNodeEditor.Duplicate();
				if (ImGui::MenuItem(ICON_FA_TRASH_CAN" Delete", "Del", nullptr, m_LuaNodeEditor.HasSelection()))
					m_LuaNodeEditor.Delete();
				ImGui::Separator();//---------------------------------------------------------------

				if (ImGui::MenuItem(ICON_FA_ARROW_POINTER" Select all", "Ctrl-A", nullptr))
					m_LuaNodeEditor.SelectAll();
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("View"))
			{
				ImGui::EndMenu();
			}
		}
		ImGui::EndMenuBar();

		ImGuiTableFlags table_flags =
			ImGuiTableFlags_Resizable;

		if (ImGui::BeginTable("##TopLevel", 2, table_flags))
		{
			ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 300.0f);
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);

			if (ImGui::TreeNodeEx("Variables", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::SameLine();
				if (ImGui::Button(ICON_FA_PLUS"##Add variable"))
				{
					std::string name = "NewVar";
					uint32_t index = 1;
					while (m_Variables.find(name + "_" + std::to_string(index)) != m_Variables.end())
					{
						index++;
					}
					name = name + "_" + std::to_string(index);
					m_Variables[name] = Variable();
				}

				if (ImGui::BeginTable("##Variables", 3, table_flags))
				{
					ImGui::TableSetupColumn("Name");
					ImGui::TableSetupColumn("Type");
					ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 24.0f);
					ImGui::TableSetupScrollFreeze(0, 1);
					ImGui::TableHeadersRow();

					static char inputBuffer[1024] = "";

					std::map<std::string, Variable>::iterator it = m_Variables.begin();
					while (it != m_Variables.end())
					{
						memset(inputBuffer, 0, sizeof(inputBuffer));
						for (int i = 0; i < it->first.length(); i++)
						{
							inputBuffer[i] = it->first[i];
						}
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						if (ImGui::InputText(std::string("##" + it->first).c_str(), inputBuffer, sizeof(inputBuffer),
							ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue
							| ImGuiInputTextFlags_CharsNoBlank))
						{
							if (m_Variables.find(inputBuffer) == m_Variables.end())
							{
								auto node = m_Variables.extract(it->first);
								std::string newName = inputBuffer;
								if (!node.empty())
								{
									if (isdigit(inputBuffer[0]))
									{
										newName.insert(0, "_");
										node.key() = newName;
									}
									else
										node.key() = newName;

									m_Variables.insert(std::move(node));
								}
								it = m_Variables.find(newName);
							}
						}

						ImGui::TableSetColumnIndex(1);
						ImGui::TableSetColumnIndex(1);
						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						std::string comboId = "##variableType" + it->first;
						ImGui::Combo(comboId.c_str(), (int*)&it->second.type,
							"Bool\0"
							"Integer\0"
							"Float\0"
							"String\0"
							"Vector 2\0"
							"Vector 3\0"
							"Quaternion\0"
							"Colour\0"
							"Object\0");
						ImGui::TableSetColumnIndex(2);
						std::string deletebuttonId = ICON_FA_TRASH_CAN"##" + it->first;
						if (ImGui::Button(deletebuttonId.c_str()))
						{
							it = m_Variables.erase(it);
						}
						else
							++it;
					}
					ImGui::EndTable();
				}
				ImGui::TreePop();
			}

			ImGui::TableSetColumnIndex(1);
			m_LuaNodeEditor.Render();
			ImGui::EndTable();
		}
	}
	m_LuaNodeEditor.ClearContext();
	ImGui::End();
}

void VisualSriptView::Save()
{
}

void VisualSriptView::SaveAs()
{
}