#include "ImGuiTilemapEditor.h"

#include "imgui/imgui.h"
#include "Scene/SceneManager.h"

#include "IconsFontAwesome6.h"
#include "IconsMaterialDesign.h"
#include "FileSystem/Directory.h"
#include "Viewers/ViewerManager.h"

#include "Engine.h"

TilemapEditor::TilemapEditor(bool* show)
	:Layer("Tilemap Editor"), m_Show(show), m_TilemapComp(nullptr)
{

}

void TilemapEditor::OnImGuiRender()
{
	PROFILE_FUNCTION();

	if (!*m_Show)
	{
		m_TilemapComp = nullptr;
		return;
	}

	if (m_TilemapComp == nullptr)
		return;

	if (m_TilemapComp->tileset)
	{
		bool updateSelectedTiles = false;
		if (m_Rows != m_TilemapComp->tileset->GetSubTexture()->GetCellsTall())
			updateSelectedTiles = true;
		else if (m_Columns != m_TilemapComp->tileset->GetSubTexture()->GetCellsWide())
			updateSelectedTiles = true;

		if (updateSelectedTiles)
		{
			m_SelectedTiles.clear();
			m_SelectedTiles.resize((size_t)(m_TilemapComp->tileset->GetSubTexture()->GetCellsTall()));
			for (size_t i = 0; i < m_TilemapComp->tileset->GetSubTexture()->GetCellsTall(); i++)
			{
				m_SelectedTiles[i].resize((size_t)(m_TilemapComp->tileset->GetSubTexture()->GetCellsWide()));
			}
			m_Rows = m_TilemapComp->tileset->GetSubTexture()->GetCellsTall();
			m_Columns = m_TilemapComp->tileset->GetSubTexture()->GetCellsWide();
		}
	}

	if (ImGui::Begin("Tilemap Editor", m_Show))
	{
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

		if (m_TilemapComp->tileset)
		{
			ImVec2 displaySize;
			if (m_TilemapComp->tileset->GetSubTexture()->GetTexture())
			{
				displaySize = ImVec2((float)m_TilemapComp->tileset->GetSubTexture()->GetTexture()->GetWidth(),
					(float)m_TilemapComp->tileset->GetSubTexture()->GetTexture()->GetHeight());
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

			ImGui::Image(m_TilemapComp->tileset->GetSubTexture()->GetTexture(), displaySize);

			ImDrawList* draw_list = ImGui::GetWindowDrawList();
			const ImU32 lineColour = ImColor(ImVec4(0.5f, 0.5f, 0.5f, 0.5f));
			const ImU32 selectionColour = ImColor(ImVec4(0.1f, 0.1f, 0.5f, 0.5f));
			const ImU32 selectionColourOutline = ImColor(ImVec4(0.2f, 0.2f, 0.7f, 0.7f));
			//Horizontal Lines
			for (uint32_t i = 0; i <= m_Rows; i++)
			{
				float y = (float)(m_TilemapComp->tileset->GetSubTexture()->GetSpriteHeight() * i);
				draw_list->AddLine(ImVec2(p.x, p.y + y), ImVec2(p.x + displaySize.x, p.y + y), lineColour);
			}
			//Vertical Lines
			for (uint32_t i = 0; i <= m_Columns; i++)
			{
				float x = (float)(m_TilemapComp->tileset->GetSubTexture()->GetSpriteWidth() * i);
				draw_list->AddLine(ImVec2(p.x + x, p.y), ImVec2(p.x + x, p.y + displaySize.y), lineColour);
			}

			ImVec2 spriteSize((float)m_TilemapComp->tileset->GetSubTexture()->GetSpriteWidth(), (float)m_TilemapComp->tileset->GetSubTexture()->GetSpriteHeight());

			if (ImGui::IsItemHovered())
			{
				static float beginClickPosX;
				static float beginClickPosY;
				static bool beginClick = false;
				if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
				{
					beginClickPosX = ImGui::GetMousePos().x - p.x;
					beginClickPosY = ImGui::GetMousePos().y - p.y;
					size_t cellX = (size_t)std::floor(beginClickPosX / m_TilemapComp->tileset->GetSubTexture()->GetSpriteWidth());
					size_t cellY = (size_t)std::floor(beginClickPosY / m_TilemapComp->tileset->GetSubTexture()->GetSpriteHeight());
					if (!ImGui::GetIO().KeyCtrl)
					{
						std::fill(m_SelectedTiles.begin(), m_SelectedTiles.end(), std::vector<bool>(m_Columns));
					}
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

					size_t cellXMin = (size_t)std::floor(std::min(beginClickPosX, currentClickPosX) / m_TilemapComp->tileset->GetSubTexture()->GetSpriteWidth());
					size_t cellYMin = (size_t)std::floor(std::min(beginClickPosY, currentClickPosY) / m_TilemapComp->tileset->GetSubTexture()->GetSpriteHeight());

					size_t cellXMax = (size_t)std::ceil(std::max(beginClickPosX, currentClickPosX) / m_TilemapComp->tileset->GetSubTexture()->GetSpriteWidth());
					size_t cellYMax = (size_t)std::ceil(std::max(beginClickPosY, currentClickPosY) / m_TilemapComp->tileset->GetSubTexture()->GetSpriteHeight());

					if (!ImGui::GetIO().KeyCtrl)
					{
						std::fill(m_SelectedTiles.begin(), m_SelectedTiles.end(), std::vector<bool>(m_Columns));
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
						ImVec2 topLeft(j * spriteSize.x + p.x, i * spriteSize.y + p.y);
						ImVec2 bottomRight(j * spriteSize.x + p.x + spriteSize.x, i * spriteSize.y + p.y + spriteSize.y);
						draw_list->AddRectFilled(topLeft, bottomRight, selectionColour);
						draw_list->AddRect(topLeft, bottomRight, selectionColourOutline);
					}
				}
			}

			ImGui::EndChild();

			ImGui::DragInt2("Coords", m_HoveredCoords);
		}
	}
	ImGui::End();
}

void TilemapEditor::OnRender(const Vector3f& mousePosition, const TransformComponent& transformComp, TilemapComponent& tilemapComp)
{
	m_TilemapComp = &tilemapComp;
	Vector3f localPosition = mousePosition * Matrix4x4::Inverse(transformComp.GetWorldMatrix());

	uint32_t cellX((uint32_t)std::floor(localPosition.x));
	uint32_t cellY((uint32_t)std::floor(-localPosition.y));

	Matrix4x4 tileTransform = transformComp.GetWorldMatrix()
		* Matrix4x4::Translate(Vector3f((float)cellX + 0.5f, -(float)cellY - 0.5f, 0.01f));

	if (cellX >= 0 && cellX < m_TilemapComp->tilesWide && cellY >= 0 && cellY < m_TilemapComp->tilesHigh)
	{
		//Renderer2D::DrawQuad(tileTransform, Colours::INDIGO);
		for (size_t y = 0; y < m_SelectedTiles.size(); y++)
		{
			for (size_t x = 0; x < m_SelectedTiles[y].size(); x++)
			{
				if (m_SelectedTiles[y][x])
				{
					if (m_TilemapComp->tileset)
						m_TilemapComp->tileset->SetCurrentTile((uint32_t)(y * m_SelectedTiles[y].size() + x));
					break;
				}
			}
		}
		Renderer2D::DrawQuad(tileTransform, m_TilemapComp->tileset->GetSubTexture());
	}

	m_HoveredCoords[0] = cellX;
	m_HoveredCoords[1] = cellY;
}

void TilemapEditor::Show()
{
	*m_Show = true;
}

void TilemapEditor::Hide()
{
	*m_Show = false;
	m_TilemapComp = nullptr;
}

bool TilemapEditor::HasSelection()
{
	for (size_t i = 0; i < m_SelectedTiles.size(); i++)
	{
		for (size_t j = 0; j < m_SelectedTiles[i].size(); j++)
		{
			if (m_SelectedTiles[i][j])
				return true;
		}
	}
	return false;
}

Matrix4x4 TilemapEditor::CalculateTransform(const TransformComponent& transformComp, uint32_t x, uint32_t y)
{
	// Calculate the 3D position of the cell

	return Matrix4x4();
}