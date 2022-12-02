#include "TilesetView.h"

#include "IconsFontAwesome6.h"

#include "Engine.h"

#include "ImGui/ImGuiTextureEdit.h"
#include "FileSystem/FileDialog.h"
#include "MainDockSpace.h"
#include "ViewerManager.h"

TilesetView::TilesetView(bool* show, std::filesystem::path filepath)
	:View("TilesetView"), m_Show(show), m_Filepath(filepath)
{
}

void TilesetView::OnAttach()
{
	m_WindowName = ICON_FA_GRIP + std::string(" " + m_Filepath.filename().string());

	m_Tileset = AssetManager::GetAsset<Tileset>(m_Filepath);
	m_LocalTileset = CreateRef<Tileset>(*m_Tileset);
	m_LocalTileset->SetSubTexture(CreateRef<SubTexture2D>(*m_LocalTileset->GetSubTexture()));

	m_SelectedTiles.resize((size_t)(m_LocalTileset->GetSubTexture()->GetCellsTall()));
	for (size_t i = 0; i < m_LocalTileset->GetSubTexture()->GetCellsTall(); i++)
	{
		m_SelectedTiles[i].resize((size_t)(m_LocalTileset->GetSubTexture()->GetCellsWide()));
	}
}

void TilesetView::OnImGuiRender()
{
	if (!*m_Show)
	{
		if (m_Dirty)
		{
			ImGui::OpenPopup("Save Prompt");
		}

		if (ImGui::BeginPopupModal("Save Prompt"))
		{
			ImGui::TextUnformatted("Save unsaved changes?");
			if (ImGui::Button("Save"))
			{
				Save();
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Don't Save"))
			{
				m_Dirty = false;
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
			{
				*m_Show = true;
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
		return;
	}

	ImGui::SetNextWindowSize(ImVec2(640, 480), ImGuiCond_FirstUseEver);

	ImVec2 displaySize;
	if (m_LocalTileset->GetSubTexture()->GetTexture())
	{
		displaySize = ImVec2((float)m_LocalTileset->GetSubTexture()->GetTexture()->GetWidth() * m_Zoom,
			(float)m_LocalTileset->GetSubTexture()->GetTexture()->GetHeight() * m_Zoom);
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
				if (ImGui::MenuItem(ICON_FA_FLOPPY_DISK" Save", "Ctrl + S"))
				{
					Save();
				}
				if (ImGui::MenuItem(ICON_FA_FILE_SIGNATURE" Save As", "Ctrl + Shift + S"))
				{
					SaveAs();
				}
				ImGui::EndMenu();
			}
		}
		ImGui::EndMenuBar();

		ImGuiTableFlags table_flags =
			ImGuiTableFlags_Resizable;
		if (ImGui::BeginTable("##TopLevel", 2, table_flags))
		{
			ImGui::TableSetupColumn("##Right", ImGuiTableColumnFlags_NoHeaderLabel);
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);

			if (ImGui::Texture2DEdit("Texture", m_LocalTileset->GetSubTexture()->GetTexture()))
			{
				m_Dirty = true;
				m_LocalTileset->GetSubTexture()->RecalculateCellsDimensions();
			}

			int spriteSize[2] = { (int)m_LocalTileset->GetSubTexture()->GetSpriteWidth(), (int)m_LocalTileset->GetSubTexture()->GetSpriteHeight() };
			if (ImGui::InputInt2("Sprite Size", spriteSize))
			{
				m_LocalTileset->GetSubTexture()->SetSpriteDimensions(spriteSize[0], spriteSize[1]);
				m_Dirty = true;
			}

			int cellsWide = m_LocalTileset->GetSubTexture()->GetCellsWide();
			int cellsTall = m_LocalTileset->GetSubTexture()->GetCellsTall();

			if (ImGui::InputInt("Cells Wide", &cellsWide))
			{
				m_LocalTileset->GetSubTexture()->SetCellDimensions(cellsWide, cellsTall);
				m_Dirty = true;
			}
			if (ImGui::InputInt("Cells Tall", &cellsTall))
			{
				m_LocalTileset->GetSubTexture()->SetCellDimensions(cellsWide, cellsTall);
				m_Dirty = true;
			}

			ImGui::Separator();

			if (ImGui::TreeNodeEx("Animations", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::SameLine();
				if (ImGui::Button(ICON_FA_PLUS"## Add animation"))
				{
					m_LocalTileset->AddAnimation("New Animation", 0, 3, 0.1f);
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
					for (auto& [name, animation] : m_LocalTileset->GetAnimations())
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
							m_LocalTileset->RenameAnimation(name, inputBuffer);
							m_Dirty = true;
							activeIndex = -1;
						}

						int frameStart = (int)animation.GetStartFrame();
						int frameCount = (int)animation.GetFrameCount();
						float frameTime = animation.GetHoldTime();

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
						if (ImGui::DragFloat(frameTimeStr.c_str(), &frameTime, 0.001f, 0.001f, 10.0f, "% .3f"))
						{
							animation.SetHoldTime(frameTime);
							m_Dirty = true;
						}

						ImGui::TableSetColumnIndex(4);
						std::string deleteStr = ICON_FA_TRASH_CAN"##" + name + std::to_string(index);
						if (ImGui::Button(deleteStr.c_str()))
							deletedAnimation = name;
						ImGui::Tooltip("Delete Animation");

						index++;
					}
					ImGui::EndTable();

					if (!deletedAnimation.empty())
						m_LocalTileset->RemoveAnimation(deletedAnimation);
				}

				ImGui::TreePop();
			}

			ImGui::TableSetColumnIndex(1);

			ImGui::SliderFloat("Zoom", &m_Zoom, 0.25f, 4.0f, "%.2f");

			if (m_LocalTileset->GetSubTexture()->GetTexture())
			{
				ImVec2 tileSize((float)m_LocalTileset->GetSubTexture()->GetSpriteWidth() * m_Zoom, (float)m_LocalTileset->GetSubTexture()->GetSpriteHeight() * m_Zoom);
				ImGuiWindowFlags window_flags_image = ImGuiWindowFlags_HorizontalScrollbar;
				ImGui::BeginChild("Tileset Texture",
					ImVec2(ImGui::GetContentRegionAvail().x - 5.0f, std::min(displaySize.y + 5.0f, ImGui::GetContentRegionAvail().y - 5.0f)),
					false, window_flags_image);
				const ImVec2 p = ImGui::GetCursorScreenPos();
				ImGui::Image(m_LocalTileset->GetSubTexture()->GetTexture(), displaySize);
				ImDrawList* draw_list = ImGui::GetWindowDrawList();

				const ImU32 lineColour = ImColor(ImVec4(0.5f, 0.5f, 0.5f, 0.5f));
				const ImU32 selectionColour = ImColor(ImVec4(0.1f, 0.1f, 0.5f, 0.5f));
				const ImU32 selectionColourOutline = ImColor(ImVec4(0.2f, 0.2f, 0.7f, 0.7f));

				//Horizontal Lines
				for (uint32_t i = 0; i <= m_LocalTileset->GetSubTexture()->GetCellsTall(); i++)
				{
					float y = tileSize.y * (float)i;
					draw_list->AddLine(ImVec2(p.x, p.y + y), ImVec2(p.x + displaySize.x, p.y + y), lineColour);
				}
				//Vertical Lines
				for (uint32_t i = 0; i <= m_LocalTileset->GetSubTexture()->GetCellsWide(); i++)
				{
					float x = tileSize.x * (float)i;
					draw_list->AddLine(ImVec2(p.x + x, p.y), ImVec2(p.x + x, p.y + displaySize.y), lineColour);

					if (i != m_LocalTileset->GetSubTexture()->GetCellsWide()
						&& tileSize.y > 20.0f)
					{
						for (uint32_t j = 0; j < m_LocalTileset->GetSubTexture()->GetCellsTall(); j++)
						{
							float y = tileSize.y * (float)j;
							std::string number = std::to_string(i + (j * m_LocalTileset->GetSubTexture()->GetCellsWide()));
							draw_list->AddText(ImVec2(p.x + x + 2, p.y + y + 2), lineColour, number.c_str());
						}
					}
				}


				if (ImGui::IsItemHovered())
				{
					static float beginClickPosX;
					static float beginClickPosY;
					static bool beginClick = false;
					if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
					{
						beginClickPosX = ImGui::GetMousePos().x - p.x;
						beginClickPosY = ImGui::GetMousePos().y - p.y;
						size_t cellX = (size_t)std::floor(beginClickPosX / tileSize.x);
						size_t cellY = (size_t)std::floor(beginClickPosY / tileSize.y);
						if (!ImGui::GetIO().KeyCtrl)
						{
							std::fill(m_SelectedTiles.begin(), m_SelectedTiles.end(), 
								std::vector<bool>(m_LocalTileset->GetSubTexture()->GetCellsWide()));
						}
						ASSERT(cellY <= m_SelectedTiles.size(), "coords must be in bounds!");
						ASSERT(cellX <= m_SelectedTiles[cellY].size(), "coords must be in bounds!");

						m_SelectedTiles[cellY][cellX] = !m_SelectedTiles[cellY][cellX];
						beginClick = true;
					}

					if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
					{
						beginClick = false;
					}

					if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
					{
						float currentClickPosX = ImGui::GetMousePos().x - p.x;
						float currentClickPosY = ImGui::GetMousePos().y - p.y;

						size_t cellXMin = (size_t)std::floor(std::min(beginClickPosX, currentClickPosX) / tileSize.x);
						size_t cellYMin = (size_t)std::floor(std::min(beginClickPosY, currentClickPosY) / tileSize.y);

						size_t cellXMax = (size_t)std::ceil(std::max(beginClickPosX, currentClickPosX) / tileSize.x);
						size_t cellYMax = (size_t)std::ceil(std::max(beginClickPosY, currentClickPosY) / tileSize.y);

						if (!ImGui::GetIO().KeyCtrl)
						{
							std::fill(m_SelectedTiles.begin(), m_SelectedTiles.end(), 
								std::vector<bool>(m_LocalTileset->GetSubTexture()->GetCellsWide()));
						}

						for (size_t i = cellYMin; i < cellYMax; i++)
						{
							for (size_t j = cellXMin; j < cellXMax; j++)
							{
								m_SelectedTiles[i][j] = true;
							}
						}
					}
				}

				for (size_t i = 0; i < m_SelectedTiles.size(); i++)
				{
					for (size_t j = 0; j < m_SelectedTiles[i].size(); j++)
					{
						if (m_SelectedTiles[i][j])
						{
							ImVec2 topLeft(j * tileSize.x + p.x, i * tileSize.y + p.y); 
							ImVec2 bottomRight(j * tileSize.x + p.x + tileSize.x, i * tileSize.y + p.y + tileSize.y);
							draw_list->AddRectFilled(topLeft, bottomRight, selectionColour);
							draw_list->AddRect(topLeft, bottomRight, selectionColourOutline);
						}
					}
				}
			}
			else
			{
				ImGui::Dummy(displaySize);
			}
			ImGui::EndChild();

			ImGui::EndTable();
		}
	}
	ImGui::End();
}

void TilesetView::Save()
{
	m_LocalTileset->Save();
	m_Tileset->Load(m_Filepath);
	m_Dirty = false;

}

void TilesetView::SaveAs()
{
	auto ext = m_LocalTileset->GetFilepath().extension();
	std::optional<std::wstring> dialogPath = FileDialog::SaveAs(L"Save As...", ConvertToWideChar(m_LocalTileset->GetFilepath().extension().string()));
	if (dialogPath)
	{
		std::filesystem::path filepath = dialogPath.value();
		if (!filepath.has_extension())
			filepath.replace_extension(ext);
		m_LocalTileset->SaveAs(filepath);
		m_Dirty = false;
	}
}