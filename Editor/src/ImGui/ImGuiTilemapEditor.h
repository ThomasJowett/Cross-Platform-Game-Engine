#pragma once
#include "Scene/Components/TilemapComponent.h"
#include "Scene/Components/TransformComponent.h"
#include "Core/Layer.h"

#include "History/HistoryCommands.h"

class TilemapEditor : public Layer
{
	enum class DrawMode
	{
		Stamp,
		Random,
		Fill,
		Rect
	};
public:
	TilemapEditor(bool* show);
	virtual void OnImGuiRender() override;
	virtual void OnEvent(Event& event) override;
	void OnRender(const Vector3f& mousePosition);
	void SetTilemapEntity(Entity tilemapEntity);
	bool HasSelection();
	bool IsShown() const { return *m_Show; }
	bool IsHovered() const;

	void Show();
	void Hide();
private:
	void FloodFillTile(uint32_t x, uint32_t y, uint32_t tileIndex);
	void FloodFillTileRecursive(uint32_t x, uint32_t y, uint32_t originalTileType, uint32_t newTileType);
	uint32_t GetRandomSelectedTile();
	void ApplyStamp(uint32_t startX, uint32_t startY, int topRightSelectionX, int topRightSelectionY);
	Matrix4x4 GetTileTransform(int x, int y);
private:
	bool* m_Show;
	DrawMode m_DrawMode = DrawMode::Stamp;

	std::vector<std::vector<bool>> m_SelectedTiles;

	uint32_t m_Rows = 0;
	uint32_t m_Columns = 0;

	std::filesystem::file_time_type m_CurrentFileTime;

	int m_HoveredCoords[2];

	Entity m_TilemapEntity;
	TilemapComponent* m_TilemapComp = nullptr;
	const TransformComponent* m_TransformComp = nullptr;

	std::pair<bool, Ref<EditComponentCommand<TilemapComponent>>> m_EditTilemapComponent;
};
