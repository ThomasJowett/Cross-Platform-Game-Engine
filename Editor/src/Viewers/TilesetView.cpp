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
	m_LocalTileset->SetCurrentTile(0);

	if (m_LocalTileset->GetSubTexture()->GetSpriteWidth() <= 32 || m_LocalTileset->GetSubTexture()->GetSpriteHeight() <= 32)
		m_Zoom = 2.0f;
	if (m_LocalTileset->GetSubTexture()->GetSpriteWidth() <= 16 || m_LocalTileset->GetSubTexture()->GetSpriteHeight() <= 16)
		m_Zoom = 4.0f;

	m_SelectedXCoord = 0;
	m_SelectedYCoord = 0;
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

		ImGuiTableFlags table_flags = ImGuiTableFlags_SizingFixedFit;
		if (ImGui::BeginTable("##TopLevel", 2, table_flags))
		{
			ImGui::TableSetupColumn("##Right", ImGuiTableColumnFlags_NoHeaderLabel);
			ImGui::TableSetupColumn("##Left", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_NoHeaderLabel);
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);

			if (ImGui::Texture2DEdit("Texture", m_LocalTileset->GetSubTexture()->GetTexture()))
			{
				m_Dirty = true;
				m_LocalTileset->SetCurrentTile(0);
				m_LocalTileset->GetSubTexture()->RecalculateCellsDimensions();
			}

			int tileSize[2] = { (int)m_LocalTileset->GetSubTexture()->GetSpriteWidth(), (int)m_LocalTileset->GetSubTexture()->GetSpriteHeight() };
			if (ImGui::InputInt2("Tile Size", tileSize))
			{
				m_LocalTileset->GetSubTexture()->SetSpriteDimensions(tileSize[0], tileSize[1]);
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

			ImGui::Button(ICON_FA_DRAW_POLYGON"##Add Polygon Collider");
			ImGui::Tooltip("Add Polygon Collider");
			ImGui::SameLine();
			ImGui::Button(ICON_FA_VECTOR_SQUARE"##Add Box Collider");
			ImGui::Tooltip("Add Box Collider");
			ImGui::SameLine();
			ImGui::Button(ICON_FA_MAGNET"##Snap");
			ImGui::Tooltip("Snapping");
			ImGui::SameLine();
			ImGui::Button(ICON_FA_TRASH"##Delete Collider");
			ImGui::Tooltip("Remove Collider");

			float ratio = ImGui::GetContentRegionAvail().x / tileSize[0];

			ImGui::Image(m_LocalTileset->GetSubTexture(), ImVec2(ImGui::GetContentRegionAvail().x, tileSize[1] * ratio));

			float probability = (float)m_LocalTileset->GetTile(m_SelectedXCoord, m_SelectedYCoord).GetProbability();
			if (ImGui::SliderFloat("Probability", &probability, 0.0f, 1.0f, "%.2f"))
			{
				m_LocalTileset->GetTile(m_SelectedXCoord, m_SelectedYCoord).SetProbability((double)probability);
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
						uint32_t cellX = (uint32_t)std::floor(beginClickPosX / tileSize.x);
						uint32_t cellY = (uint32_t)std::floor(beginClickPosY / tileSize.y);

						m_LocalTileset->SetCurrentTile(cellX, cellY);
						m_SelectedXCoord = cellX;
						m_SelectedYCoord = cellY;
						beginClick = true;
					}

					if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
					{
						beginClick = false;
					}
				}

				ImVec2 topLeft(m_SelectedXCoord * tileSize.x + p.x, m_SelectedYCoord * tileSize.y + p.y);
				ImVec2 bottomRight(m_SelectedXCoord * tileSize.x + p.x + tileSize.x, m_SelectedYCoord * tileSize.y + p.y + tileSize.y);
				draw_list->AddRectFilled(topLeft, bottomRight, selectionColour);
				draw_list->AddRect(topLeft, bottomRight, selectionColourOutline);

				ImGui::EndChild();
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