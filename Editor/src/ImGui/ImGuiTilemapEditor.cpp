#include "ImGuiTilemapEditor.h"

#include "ImGui/imgui.h"
#include "Scene/SceneManager.h"

#include "IconsFontAwesome5.h"
#include "IconsMaterialDesign.h"
#include "FileSystem/Directory.h"
#include "Viewers/ViewerManager.h"

#include "Engine.h"

void TilemapEditor::OnImGuiRender(TilemapComponent& tilemap)
{
	if (tilemap.tileset)
	{
		bool updateSelectedTiles = false;
		if (m_Rows != tilemap.tileset->GetSubTexture()->GetCellsTall())
			updateSelectedTiles = true;
		else if (m_Columns != tilemap.tileset->GetSubTexture()->GetCellsWide())
			updateSelectedTiles = true;

		std::filesystem::file_time_type lastWrittenTime = std::filesystem::last_write_time(tilemap.tileset->GetFilepath());

		if (lastWrittenTime > m_CurrentFileTime)
		{
			tilemap.tileset->Load();
			updateSelectedTiles = true;
			m_CurrentFileTime = lastWrittenTime;
		}

		if (updateSelectedTiles)
		{
			m_SelectedTiles.clear();
			m_SelectedTiles.resize((size_t)(tilemap.tileset->GetSubTexture()->GetCellsTall()));
			for (size_t i = 0; i < tilemap.tileset->GetSubTexture()->GetCellsTall(); i++)
			{
				m_SelectedTiles[i].resize((size_t)(tilemap.tileset->GetSubTexture()->GetCellsWide()));
			}
			m_Rows = tilemap.tileset->GetSubTexture()->GetCellsTall();
			m_Columns = tilemap.tileset->GetSubTexture()->GetCellsWide();
		}
	}

	if (ImGui::Combo("Orientation", (int*)&tilemap.orientation,
		"Orthogonal\0"
		"Isometric\0"
		"Isometric (staggered)\0"
		"Hexagonal (staggered)"))
	{
		SceneManager::CurrentScene()->MakeDirty();
	}

	int tilesWide = tilemap.tilesWide;
	if (ImGui::DragInt("Width", &tilesWide, 1.0f, 0, 1000))
	{
		tilemap.tilesWide = tilesWide;

		for (auto& row : tilemap.tiles)
		{
			row.resize(tilesWide);
		}

		SceneManager::CurrentScene()->MakeDirty();
	}

	int tilesHigh = tilemap.tilesHigh;
	if (ImGui::DragInt("Height", &tilesHigh, 1.0f, 0, 1000))
	{

		tilemap.tiles.resize(tilesHigh);
		if (tilemap.tilesHigh < tilesHigh)
		{
			for (size_t i = tilemap.tilesHigh; i < tilesHigh; i++)
			{
				tilemap.tiles[i].resize(tilemap.tilesWide);
			}
		}
		tilemap.tilesHigh = tilesHigh;

		SceneManager::CurrentScene()->MakeDirty();
	}

	std::string tilesetName;
	if (tilemap.tileset)
		tilesetName = tilemap.tileset->GetFilepath().filename().string();

	if (ImGui::BeginCombo("Tileset", tilesetName.c_str()))
	{
		for (std::filesystem::path& file : Directory::GetFilesRecursive(Application::GetOpenDocumentDirectory(), ViewerManager::GetExtensions(FileType::TILESET)))
		{
			const bool is_selected = false;
			if (ImGui::Selectable(file.filename().string().c_str(), is_selected))
			{
				if (!tilemap.tileset)
					tilemap.tileset = CreateRef<Tileset>();
				tilemap.tileset->Load(file);
				SceneManager::CurrentScene()->MakeDirty();
			}
			ImGui::Tooltip(file.string().c_str());
		}
		ImGui::EndCombo();
	}
	if (tilemap.tileset)
	{
		ImGui::SameLine();
		if (ImGui::Button(ICON_FA_PEN_SQUARE))
		{
			ViewerManager::OpenViewer(tilemap.tileset->GetFilepath());
		}
	}

	bool selected = false;

	// Stamp ---------------------------------
	selected = m_DrawMode == DrawMode::Stamp;
	if (!selected)
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
	if (ImGui::Button(ICON_FA_PEN))
		m_DrawMode = DrawMode::Stamp;
	ImGui::Tooltip("Stamp Brush");
	if (!selected)
		ImGui::PopStyleColor();
	// Random ---------------------------------
	ImGui::SameLine();
	selected = m_DrawMode == DrawMode::Random;
	if (!selected)
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
	if (ImGui::Button(ICON_FA_DICE))
		m_DrawMode = DrawMode::Random;
	ImGui::Tooltip("Random Brush");
	if (!selected)
		ImGui::PopStyleColor();
	// Fill ---------------------------------
	ImGui::SameLine();
	selected = m_DrawMode == DrawMode::Fill;
	if (!selected)
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
	if (ImGui::Button(ICON_FA_FILL_DRIP))
		m_DrawMode = DrawMode::Fill;
	ImGui::Tooltip("Bucket Fill Tool");
	if (!selected)
		ImGui::PopStyleColor();
	// Rect ---------------------------------
	ImGui::SameLine();
	selected = m_DrawMode == DrawMode::Rect;
	if (!selected)
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
	if (ImGui::Button(ICON_FA_SQUARE))
		m_DrawMode = DrawMode::Rect;
	ImGui::Tooltip("Shape Fill Tool");
	if (!selected)
		ImGui::PopStyleColor();

	if (tilemap.tileset)
	{
		ImVec2 displaySize;
		if (tilemap.tileset->GetSubTexture()->GetTexture())
		{
			displaySize = ImVec2((float)tilemap.tileset->GetSubTexture()->GetTexture()->GetWidth(),
				(float)tilemap.tileset->GetSubTexture()->GetTexture()->GetHeight());
		}
		else
		{
			displaySize = ImVec2(100, 100);
		}
		ImGuiWindowFlags window_flags_image = ImGuiWindowFlags_HorizontalScrollbar;
		ImGui::BeginChild("Tileset Texture",
			ImVec2(ImGui::GetContentRegionAvail().x, displaySize.y + 5.0f),
			false, window_flags_image);
		const ImVec2 p = ImGui::GetCursorScreenPos();

		
		ImGui::Image(tilemap.tileset->GetSubTexture()->GetTexture(), displaySize);

		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		const ImU32 lineColour = ImColor(ImVec4(0.5f, 0.5f, 0.5f, 0.5f));
		const ImU32 selectionColour = ImColor(ImVec4(0.1f, 0.1f, 0.5f, 0.5f));
		const ImU32 selectionColourOutline = ImColor(ImVec4(0.2f, 0.2f, 0.7f, 0.7f));
		//Horizontal Lines
		for (uint32_t i = 0; i <= m_Rows; i++)
		{
			float y = (float)(tilemap.tileset->GetSubTexture()->GetSpriteHeight() * i);
			draw_list->AddLine(ImVec2(p.x, p.y + y), ImVec2(p.x + displaySize.x, p.y + y), lineColour);
		}
		//Vertical Lines
		for (uint32_t i = 0; i <= m_Columns; i++)
		{
			float x = (float)(tilemap.tileset->GetSubTexture()->GetSpriteWidth() * i);
			draw_list->AddLine(ImVec2(p.x + x, p.y), ImVec2(p.x + x, p.y + displaySize.y), lineColour);
		}

		ImVec2 spriteSize((float)tilemap.tileset->GetSubTexture()->GetSpriteWidth(), (float)tilemap.tileset->GetSubTexture()->GetSpriteHeight());

		static bool drawrect = false;
		if (ImGui::IsItemHovered())
		{
			if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{
				float clickPosX = ImGui::GetMousePos().x - p.x;
				float clickPosY = ImGui::GetMousePos().y - p.y;
				size_t cellX = (size_t)std::floor(clickPosX / tilemap.tileset->GetSubTexture()->GetSpriteWidth());
				size_t cellY = (size_t)std::floor(clickPosY / tilemap.tileset->GetSubTexture()->GetSpriteHeight());
				if (!ImGui::GetIO().KeyCtrl)
				{
					std::fill(m_SelectedTiles.begin(), m_SelectedTiles.end(), std::vector<bool>(m_Columns));
				}
				m_SelectedTiles[cellY][cellX] = !m_SelectedTiles[cellY][cellX];
			}
		}
		for (size_t i = 0; i < m_SelectedTiles.size(); i++)
		{
			for (size_t j = 0; j < m_SelectedTiles[i].size(); j++)
			{
				if (m_SelectedTiles[i][j])
				{
					ImVec2 topLeft(j * spriteSize.x + p.x, i * spriteSize.y + p.y);
					ImVec2 bottomRight(j * spriteSize.x + p.x + spriteSize.x, i * spriteSize.y + p.y + spriteSize.y);
					draw_list->AddRectFilled(topLeft, bottomRight, selectionColour);
					draw_list->AddRect(topLeft, bottomRight, selectionColourOutline);
				}
			}
		}

		ImGui::EndChild();
	}
	// TODO: display the tileset pallette
}
