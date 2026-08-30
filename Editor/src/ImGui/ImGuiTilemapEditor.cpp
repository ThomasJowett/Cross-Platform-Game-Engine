#include "ImGuiTilemapEditor.h"

#include "imgui/imgui.h"
#include "Scene/SceneManager.h"

#include "IconsFontAwesome6.h"
#include "IconsMaterialDesign.h"
#include "FileSystem/Directory.h"
#include "Viewers/ViewerManager.h"
#include "Events/SceneEvent.h"

#include "Renderer/Renderer2D.h"
#include "Core/Input.h"
#include "Core/KeyCodes.h"
#include "Core/MouseButtonCodes.h"
#include "ImGui/ImGuiUtilities.h"

TilemapEditor::TilemapEditor(bool* show)
	:Layer("Tilemap Editor"), m_Show(show)
{
	m_HoveredCoords[0] = -1;
	m_HoveredCoords[1] = -1;
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
					size_t cellX = (size_t)std::floor(beginClickPosX / spriteSize.x);
					size_t cellY = (size_t)std::floor(beginClickPosY / spriteSize.y);
					if (!ImGui::GetIO().KeyCtrl)
					{
						for (auto& row : m_SelectedTiles)
						{
							std::fill(row.begin(), row.end(), false);
						}
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

					size_t cellXMin = (size_t)std::floor(std::min(beginClickPosX, currentClickPosX) / spriteSize.x);
					size_t cellYMin = (size_t)std::floor(std::min(beginClickPosY, currentClickPosY) / spriteSize.y);

					size_t cellXMax = (size_t)std::ceil(std::max(beginClickPosX, currentClickPosX) / spriteSize.x);
					size_t cellYMax = (size_t)std::ceil(std::max(beginClickPosY, currentClickPosY) / spriteSize.y);

					if (!ImGui::GetIO().KeyCtrl)
					{
						for (auto& row : m_SelectedTiles)
						{
							std::fill(row.begin(), row.end(), false);
						}
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

void TilemapEditor::OnEvent(Event& e)
{
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<SceneChangedEvent>([this](SceneChangedEvent& event) {
		Hide();
		return false;
		});

	dispatcher.Dispatch<MouseButtonPressedEvent>([this](MouseButtonPressedEvent& event) {
		return false;
		});

	dispatcher.Dispatch<MouseButtonReleasedEvent>([](MouseButtonReleasedEvent& event) {
		return false;
		});
}

void TilemapEditor::OnRender(const Vector3f& mousePosition)
{
	if (!m_TilemapComp || !m_TransformComp || !m_TilemapComp->tileset)
		return;

	Vector3f localPosition = mousePosition * Matrix4x4::Inverse(m_TransformComp->GetWorldMatrix());
	Matrix4x4 tileTransform;

	if (m_TilemapComp->orientation == TilemapComponent::Orientation::orthogonal)
	{
		m_HoveredCoords[0] = (int)std::floor(localPosition.x);
		m_HoveredCoords[1] = (int)std::floor(-localPosition.y);
	}
	else if (m_TilemapComp->orientation == TilemapComponent::Orientation::isometric)
	{
		Vector2f coords = m_TilemapComp->WorldToIso(Vector2f(localPosition.x, localPosition.y));
		m_HoveredCoords[0] = (int)std::floor(coords.x);
		m_HoveredCoords[1] = (int)std::floor(coords.y);
	}
	else if (m_TilemapComp->orientation == TilemapComponent::Orientation::hexagonal)
	{
		Vector2f coords = m_TilemapComp->WorldToHex(Vector2f(localPosition.x, localPosition.y));
		m_HoveredCoords[0] = (int)std::floor(coords.x);
		m_HoveredCoords[1] = (int)std::floor(coords.y);
	}

	size_t topLeftSelectionIndex = 0;
	size_t topLeftSelectionX = std::numeric_limits<size_t>::max();
	size_t topLeftSelectionY = std::numeric_limits<size_t>::max();

	if (IsHovered())
	{
		if (Input::IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !m_EditTilemapComponent.first && !Input::IsKeyPressed(KEY_LEFT_ALT))
		{
			m_EditTilemapComponent.second = CreateRef<EditComponentCommand<TilemapComponent>>(m_TilemapEntity);
			m_EditTilemapComponent.first = true;
			m_RectStartCoords[0] = m_HoveredCoords[0];
			m_RectStartCoords[1] = m_HoveredCoords[1];
		}
		else if (Input::IsKeyPressed(KEY_LEFT_SHIFT) && !m_EditTilemapComponent.first && !Input::IsKeyPressed(KEY_LEFT_ALT))
		{
			m_RectStartCoords[0] = m_HoveredCoords[0];
			m_RectStartCoords[1] = m_HoveredCoords[1];
		}

		if (Input::IsKeyPressed(KEY_LEFT_SHIFT))
		{
			if (m_DrawMode == TilemapEditor::DrawMode::Rect)
			{
				int minX = std::max(0, std::min(m_RectStartCoords[0], m_HoveredCoords[0]));
				int maxX = std::min((int)m_TilemapComp->tilesWide - 1, std::max(m_RectStartCoords[0], m_HoveredCoords[0]));
				int minY = std::max(0, std::min(m_RectStartCoords[1], m_HoveredCoords[1]));
				int maxY = std::min((int)m_TilemapComp->tilesHigh - 1, std::max(m_RectStartCoords[1], m_HoveredCoords[1]));

				for (int y = minY; y <= maxY; y++)
				{
					for (int x = minX; x <= maxX; x++)
					{
						tileTransform = GetTileTransform(x, y);
						Renderer2D::DrawQuad(tileTransform, Colour(1.0f, 0.0f, 0.0f, 0.6f));
					}
				}
			}
			else
			{
				tileTransform = GetTileTransform(m_HoveredCoords[0], m_HoveredCoords[1]);
				Renderer2D::DrawQuad(tileTransform, Colour(1.0f, 0.0f, 0.0f, 0.6f));
			}
		}
		else if (HasSelection())
		{
			for (size_t y = 0; y < m_SelectedTiles.size(); y++)
			{
				for (size_t x = 0; x < (uint32_t)m_SelectedTiles[y].size(); x++)
				{
					if (m_SelectedTiles[y][x])
					{
						if (x < topLeftSelectionX)
							topLeftSelectionX = x;
						if (y < topLeftSelectionY)
							topLeftSelectionY = y;
					}
				}
			}

			topLeftSelectionIndex = (topLeftSelectionY * m_SelectedTiles[topLeftSelectionY].size() + topLeftSelectionX) + 1;
			if (m_TilemapComp->tileset) {
				if (m_DrawMode == TilemapEditor::DrawMode::Stamp)
				{
					for (size_t row = topLeftSelectionY; row < m_Rows; ++row) {
						for (size_t col = topLeftSelectionX; col < m_Columns; ++col) {
							if (m_SelectedTiles[row][col])
							{
								int tileX = m_HoveredCoords[0] + (int)(col - topLeftSelectionX);
								int tileY = m_HoveredCoords[1] + (int)(row - topLeftSelectionY);
								if (tileX < (int)m_TilemapComp->tilesWide && tileY < (int)m_TilemapComp->tilesHigh) {
									tileTransform = GetTileTransform(tileX, tileY);
									m_TilemapComp->tileset->SetCurrentTile((uint32_t)(row * m_Columns + col));
									Renderer2D::DrawQuad(tileTransform, m_TilemapComp->tileset->GetSubTexture());
								}
							}
						}
					}
				}
				else if (m_DrawMode == TilemapEditor::DrawMode::Rect && Input::IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
				{
					int minX = std::min(m_RectStartCoords[0], m_HoveredCoords[0]);
					int maxX = std::max(m_RectStartCoords[0], m_HoveredCoords[0]);
					int minY = std::min(m_RectStartCoords[1], m_HoveredCoords[1]);
					int maxY = std::max(m_RectStartCoords[1], m_HoveredCoords[1]);

					m_TilemapComp->tileset->SetCurrentTile((uint32_t)(topLeftSelectionY * m_SelectedTiles[topLeftSelectionY].size() + topLeftSelectionX));
					for (int y = minY; y <= maxY; y++) {
						for (int x = minX; x <= maxX; x++) {
							tileTransform = GetTileTransform(x, y);
							Renderer2D::DrawQuad(tileTransform, m_TilemapComp->tileset->GetSubTexture());
						}
					}
				}
				else
				{
					m_TilemapComp->tileset->SetCurrentTile((uint32_t)(topLeftSelectionY * m_SelectedTiles[topLeftSelectionY].size() + topLeftSelectionX));
					tileTransform = GetTileTransform(m_HoveredCoords[0], m_HoveredCoords[1]);
					Renderer2D::DrawQuad(tileTransform, m_TilemapComp->tileset->GetSubTexture());
				}
			}
		}

		if (Input::IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
		{
			if (Input::IsKeyPressed(KEY_LEFT_ALT))
			{
				uint32_t pickedTile = m_TilemapComp->tiles[m_HoveredCoords[1]][m_HoveredCoords[0]];

				if (pickedTile > 0)
				{
					div_t div = std::div((int)pickedTile - 1, m_Columns);
					for (auto& row : m_SelectedTiles)
					{
						std::fill(row.begin(), row.end(), false);
					}
					m_SelectedTiles[div.quot][div.rem] = true;
				}
			}
			else if (Input::IsKeyPressed(KEY_LEFT_SHIFT))
			{
				switch (m_DrawMode)
				{
				case TilemapEditor::DrawMode::Stamp:
				case TilemapEditor::DrawMode::Random:
					m_TilemapComp->tiles[m_HoveredCoords[1]][m_HoveredCoords[0]] = 0;
					break;
				case TilemapEditor::DrawMode::Fill:
					FloodFillTile(m_HoveredCoords[0], m_HoveredCoords[1], 0);
					break;
				case TilemapEditor::DrawMode::Rect:
					break;

				}
				m_TilemapComp->Rebuild();
			}
			else if (HasSelection())
			{
				switch (m_DrawMode)
				{
				case TilemapEditor::DrawMode::Stamp:
					ApplyStamp(m_HoveredCoords[0], m_HoveredCoords[1], topLeftSelectionX, topLeftSelectionY);
					break;
				case TilemapEditor::DrawMode::Random:
					m_TilemapComp->tiles[m_HoveredCoords[1]][m_HoveredCoords[0]] = GetRandomSelectedTile();
					break;
				case TilemapEditor::DrawMode::Fill:
					FloodFillTile(m_HoveredCoords[0], m_HoveredCoords[1], (uint32_t)topLeftSelectionIndex);
					break;
				case TilemapEditor::DrawMode::Rect:
					break;
				default:
					break;
				}
				m_TilemapComp->Rebuild();
			}
		}
	}

	if (!Input::IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && m_EditTilemapComponent.first)
	{
		if (m_DrawMode == DrawMode::Rect)
		{
			int minX = std::max(0, std::min(m_RectStartCoords[0], m_HoveredCoords[0]));
			int maxX = std::min((int)m_TilemapComp->tilesWide - 1, std::max(m_RectStartCoords[0], m_HoveredCoords[0]));
			int minY = std::max(0, std::min(m_RectStartCoords[1], m_HoveredCoords[1]));
			int maxY = std::min((int)m_TilemapComp->tilesHigh - 1, std::max(m_RectStartCoords[1], m_HoveredCoords[1]));

			if (Input::IsKeyPressed(KEY_LEFT_SHIFT))
			{
				for (int y = minY; y <= maxY; y++)
				{
					for (int x = minX; x <= maxX; x++)
					{
						m_TilemapComp->tiles[y][x] = 0;
					}
				}
			}
			else
			{
				for (int y = minY; y <= maxY; y++)
				{
					for (int x = minX; x <= maxX; x++)
					{
						m_TilemapComp->tiles[y][x] = GetRandomSelectedTile();
					}
				}
			}
			m_TilemapComp->Rebuild();
		}

		HistoryManager::AddHistoryRecord(m_EditTilemapComponent.second);
		m_EditTilemapComponent.first = false;
		m_EditTilemapComponent.second = nullptr;
	}
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

void TilemapEditor::FloodFillTile(uint32_t x, uint32_t y, uint32_t newTileType)
{
	uint32_t originalTileType = m_TilemapComp->tiles[y][x];
	if (originalTileType == newTileType && newTileType == 0) // Only skip if erasing and already 0
		return;

	// Use iterative approach to avoid stack overflow
	std::queue<std::pair<uint32_t, uint32_t>> queue;
	queue.push({ x, y });

	std::vector<bool> visited(m_TilemapComp->tilesWide * m_TilemapComp->tilesHigh, false);
	visited[y * m_TilemapComp->tilesWide + x] = true;

	bool isErase = (newTileType == 0);

	while (!queue.empty())
	{
		auto [currX, currY] = queue.front();
		queue.pop();

		if (m_TilemapComp->tiles[currY][currX] != originalTileType)
			continue;

		m_TilemapComp->tiles[currY][currX] = isErase ? 0 : GetRandomSelectedTile();

		auto addNeighbor = [&](int nx, int ny) {
			if (nx >= 0 && nx < (int)m_TilemapComp->tilesWide && ny >= 0 && ny < (int)m_TilemapComp->tilesHigh)
			{
				if (!visited[ny * m_TilemapComp->tilesWide + nx] && m_TilemapComp->tiles[ny][nx] == originalTileType)
				{
					visited[ny * m_TilemapComp->tilesWide + nx] = true;
					queue.push({ (uint32_t)nx, (uint32_t)ny });
				}
			}
		};

		addNeighbor((int)currX + 1, (int)currY);
		addNeighbor((int)currX - 1, (int)currY);
		addNeighbor((int)currX, (int)currY + 1);
		addNeighbor((int)currX, (int)currY - 1);
	}
}

uint32_t TilemapEditor::GetRandomSelectedTile()
{
	std::vector<uint32_t> selection;
	for (size_t i = 0; i < m_SelectedTiles.size(); i++)
	{
		for (size_t j = 0; j < m_SelectedTiles[i].size(); j++)
		{
			if (m_SelectedTiles[i][j])
				selection.push_back((uint32_t)(i * m_SelectedTiles[i].size() + j));
		}
	}

	if (selection.empty())
		return 0;

	if (selection.size() == 1)
		return selection.at(0) + 1;

	float totalProbabilites = 0.0f;

	for (uint32_t index : selection) {
		const Tile& tile = m_TilemapComp->tileset->GetTile(index);
		totalProbabilites += (float)tile.GetProbability();
	}

	float randomValue = Random::FloatInRange(0.0f, totalProbabilites);

	float cumulativeProbability = 0.0f;
	for (uint32_t index : selection) {
		const Tile& tile = m_TilemapComp->tileset->GetTile(index);
		cumulativeProbability += (float)tile.GetProbability();
		if (randomValue <= cumulativeProbability) {
			return index + 1;
		}
	}

	return 0;
}

void TilemapEditor::ApplyStamp(uint32_t startX, uint32_t startY, size_t topRightSelectionX, size_t topRightSelectionY)
{
	for (size_t row = topRightSelectionY; row < m_Rows; ++row) {

		for (size_t col = topRightSelectionX; col < m_Columns; ++col)
		{
			if (m_SelectedTiles[row][col])
			{
				size_t tileX = startX + (col - topRightSelectionX);
				size_t tileY = startY + (row - topRightSelectionY);

				if (tileX < m_TilemapComp->tilesWide &&
					tileY < m_TilemapComp->tilesHigh)
				{
					m_TilemapComp->tiles[tileY][tileX] = (uint32_t)(row * m_Columns + col + 1);
				}
			}
		}
	}
}

Matrix4x4 TilemapEditor::GetTileTransform(int x, int y)
{
	switch (m_TilemapComp->orientation)
	{
	case TilemapComponent::Orientation::orthogonal:
	{
		return m_TransformComp->GetWorldMatrix()
			* Matrix4x4::Translate(Vector3f((float)x + 0.5f, -(float)y - 0.5f, 0.01f));
	}
	case TilemapComponent::Orientation::isometric:
	{
		Vector2f isoPosition = m_TilemapComp->IsoToWorld(x, y);

		return m_TransformComp->GetWorldMatrix()
			* Matrix4x4::Translate(Vector3f(isoPosition.x, isoPosition.y, 0.01f));
	}
	case TilemapComponent::Orientation::hexagonal:
	{
		Vector2f hexPosition = m_TilemapComp->HexToWorld(x, y);
		return m_TransformComp->GetWorldMatrix()
			* Matrix4x4::Translate(Vector3f(hexPosition.x, hexPosition.y, 0.01f));
	}
	default:
		return Matrix4x4();
	}
}

void TilemapEditor::SetTilemapEntity(Entity tilemapEntity)
{
	m_TilemapComp = &(tilemapEntity.GetComponent<TilemapComponent>());
	m_TransformComp = &(tilemapEntity.GetComponent<TransformComponent>());
	m_TilemapEntity = tilemapEntity;
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

bool TilemapEditor::IsHovered() const
{
	return *m_Show
		&& m_HoveredCoords[0] >= 0
		&& m_HoveredCoords[0] < (int)m_TilemapComp->tilesWide
		&& m_HoveredCoords[1] >= 0
		&& m_HoveredCoords[1] < (int)m_TilemapComp->tilesHigh;
}