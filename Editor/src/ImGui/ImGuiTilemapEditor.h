#pragma once
#include "Scene/Components/TilemapComponent.h"
#include "Scene/Components/TransformComponent.h"

class TilemapEditor
{
	enum class DrawMode
	{
		Stamp,
		Random,
		Fill,
		Rect
	};
public:
	void OnImGuiRender(TilemapComponent& tilemap);
	void OnRender(const Vector3f& mousePosition, const TransformComponent& transformComp, TilemapComponent& tilemapComp);

	bool HasSelection();
private:

	Matrix4x4 CalculateTransform(const TransformComponent& transformComp, uint32_t x, uint32_t y);
	DrawMode m_DrawMode = DrawMode::Stamp;

	std::vector<std::vector<bool>> m_SelectedTiles;

	uint32_t m_Rows = 0;
	uint32_t m_Columns = 0;

	std::filesystem::file_time_type m_CurrentFileTime;

	int m_HoveredCoords[2];
};
