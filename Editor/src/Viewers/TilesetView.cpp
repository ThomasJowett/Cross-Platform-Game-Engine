#include "TilesetView.h"

#include "IconsFontAwesome5.h"

#include "Engine.h"

#include "ImGui/ImGuiTextureEdit.h"
#include "FileSystem/FileDialog.h"
#include "MainDockSpace.h"

TilesetView::TilesetView(bool* show, std::filesystem::path filepath)
	:Layer("TilesetView"), m_Show(show), m_Filepath(filepath)
{
}

void TilesetView::OnAttach()
{
	m_WindowName = ICON_FA_TH + std::string(" " + m_Filepath.filename().string());

	m_Tileset = CreateRef<Tileset>(m_Filepath);
}

void TilesetView::OnImGuiRender()
{
	if (!*m_Show)
		return;


	ImVec2 displaySize;
	if (m_Tileset->GetSubTexture()->GetTexture())
	{
		displaySize = ImVec2((float)m_Tileset->GetSubTexture()->GetTexture()->GetWidth() * m_Zoom,
			(float)m_Tileset->GetSubTexture()->GetTexture()->GetHeight() * m_Zoom);
	}
	else
	{
		displaySize = ImVec2(100.0f, 100.0f);
	}

	ImGuiWindowFlags flags = ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_MenuBar;

	if (m_Dirty)
		flags |= ImGuiWindowFlags_UnsavedDocument;

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
				if (ImGui::MenuItem(ICON_FA_SAVE" Save", "Ctrl + S"))
				{
					m_Tileset->Save();
					m_Dirty = false;
				}
				if (ImGui::MenuItem(ICON_FA_FILE_SIGNATURE" Save As", "Ctrl + Shift + S"))
				{
					auto ext = m_Tileset->GetFilepath().extension();
					std::optional<std::wstring> dialogPath = FileDialog::SaveAs(L"Save As...", ConvertToWideChar(m_Tileset->GetFilepath().extension().string()));
					if (dialogPath)
					{
						std::filesystem::path filepath = dialogPath.value();
						if (!filepath.has_extension())
							filepath.replace_extension(ext);
						m_Tileset->Save(filepath);
						m_Dirty = false;
					}
				}
				ImGui::EndMenu();
			}
		}
		ImGui::EndMenuBar();

		ImGuiTableFlags table_flags =
			ImGuiTableFlags_Resizable;
		if (ImGui::BeginTable("##TopLevel", 2, table_flags))
		{
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			char nameBuffer[256];
			memset(nameBuffer, 0, sizeof(nameBuffer));
			std::strncpy(nameBuffer, m_Tileset->GetName().c_str(), sizeof(nameBuffer));

			if (ImGui::InputText("Name", nameBuffer, sizeof(nameBuffer)))
			{
				m_Tileset->SetName(nameBuffer);
				m_Dirty = true;
			}

			if (ImGui::Texture2DEdit("Texture", m_Tileset->GetSubTexture()->GetTexture()))
			{
				m_Dirty = true;
				m_Tileset->GetSubTexture()->RecalculateCellsDimensions();
			}

			int spriteSize[2] = { (int)m_Tileset->GetSubTexture()->GetSpriteWidth(), (int)m_Tileset->GetSubTexture()->GetSpriteHeight() };
			if (ImGui::InputInt2("Sprite Size", spriteSize))
			{
				m_Tileset->GetSubTexture()->SetSpriteDimensions(spriteSize[0], spriteSize[1]);
				m_Dirty = true;
			}

			ImGui::Separator();

			ImGui::Text("Animations");
			ImGui::SameLine();
			if (ImGui::Button(ICON_FA_PLUS"## Add animation"))
			{
				m_Tileset->AddAnimation("New Animation", 0, 3, 0.1f);
				m_Dirty = true;
			}
			ImGui::Tooltip("Add animation");

			if (ImGui::BeginTable("Animations", 5, table_flags))
			{
				ImGui::TableSetupColumn("Name");
				ImGui::TableSetupColumn("Start Frame");
				ImGui::TableSetupColumn("Frame Count");
				ImGui::TableSetupColumn("Frame Time (ms)");
				ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 24.0f);
				ImGui::TableSetupScrollFreeze(0, 1);
				ImGui::TableHeadersRow();

				static char inputBuffer[1024] = "";

				static int activeIndex = -1;

				std::string deletedAnimation;

				int index = 0;
				for (auto& [name, animation] : m_Tileset->GetAnimations())
				{
					memset(inputBuffer, 0, sizeof(inputBuffer));
					for (int i = 0; i < name.length(); i++)
					{
						inputBuffer[i] = name[i];
					}
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
					std::string nameStr = "##name" + std::to_string(index);
					ImGui::InputText(nameStr.c_str(), inputBuffer, sizeof(inputBuffer), 
						ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue);

					if (ImGui::IsItemActive())
					{
						activeIndex = index;
					}

					if (activeIndex == index && !ImGui::IsItemActive())
					{
						m_Tileset->RenameAnimation(name, inputBuffer);
						m_Dirty = true;
						activeIndex = -1;
					}

					int frameStart = (int)animation.GetStartFrame();
					int frameCount = (int)animation.GetFrameCount();
					float frameTime = animation.GetFrameTime();

					ImGui::TableSetColumnIndex(1);
					ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
					std::string starFrameStr = "##startFrame" + std::to_string(index);
					if (ImGui::DragInt(starFrameStr.c_str(), &frameStart))
					{
						if (frameStart < 0)
							frameStart = 0;
						animation.SetStartFrame((uint32_t)frameStart);
						m_Dirty = true;
					}

					ImGui::TableSetColumnIndex(2);
					ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
					std::string frameCountStr = "##frameCount" + std::to_string(index);
					if (ImGui::DragInt(frameCountStr.c_str(), &frameCount))
					{
						if (frameCount < 0)
							frameCount = 0;
						animation.SetFrameCount((uint32_t)frameCount);
						m_Dirty = true;
					}

					ImGui::TableSetColumnIndex(3);
					ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
					std::string frameTimeStr = "##frameTime" + std::to_string(index);
					if (ImGui::DragFloat(frameTimeStr.c_str(), &frameTime, 0.001f, 0.0f, 10.0f, "% .3f"))
					{
						animation.SetFrameTime(frameTime);
						m_Dirty = true;
					}

					ImGui::TableSetColumnIndex(4);
					std::string deleteStr = ICON_FA_TRASH_ALT"##" + name + std::to_string(index);
					if(ImGui::Button(deleteStr.c_str()))
						deletedAnimation = name;
					ImGui::Tooltip("Delete Animation");

					index++;
				}
				ImGui::EndTable();

				if (!deletedAnimation.empty())
					m_Tileset->RemoveAnimation(deletedAnimation);
			}

			ImGui::TableSetColumnIndex(1);

			ImGui::SliderFloat("Zoom", &m_Zoom, 0.25f, 4.0f, "%.2f");

			if (m_Tileset->GetSubTexture()->GetTexture())
			{
				const ImVec2 p = ImGui::GetCursorScreenPos();
				ImGui::Image(m_Tileset->GetSubTexture()->GetTexture(), displaySize);
				ImDrawList* draw_list = ImGui::GetWindowDrawList();

				const ImU32 colour = ImColor(ImVec4(0.5f, 0.5f, 0.5f, 0.5f));
				//Horizontal Lines
				for (uint32_t i = 0; i <= m_Tileset->GetSubTexture()->GetCellsTall(); i++ )
				{
					float y = (float)(m_Tileset->GetSubTexture()->GetSpriteHeight()* i) * m_Zoom ;
					draw_list->AddLine(ImVec2(p.x, p.y + y), ImVec2(p.x + displaySize.x, p.y + y), colour);
				}
				//Vertical Lines
				for (uint32_t i = 0; i <= m_Tileset->GetSubTexture()->GetCellsWide(); i++)
				{
					float x = (float)(m_Tileset->GetSubTexture()->GetSpriteWidth() * i)* m_Zoom;
					draw_list->AddLine(ImVec2(p.x + x, p.y), ImVec2(p.x + x, p.y + displaySize.y), colour);
				}
			}
			else
			{
				ImGui::Dummy(displaySize);
			}

			ImGui::EndTable();
		}
	}
	ImGui::End();
}

void TilesetView::Save()
{
}

void TilesetView::SaveAs()
{
}
