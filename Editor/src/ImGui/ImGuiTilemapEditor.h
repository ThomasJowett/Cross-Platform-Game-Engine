#pragma once
#include "Scene/Components/TilemapComponent.h"

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
private:
	DrawMode m_DrawMode = DrawMode::Stamp;

	std::vector<std::vector<bool>> m_SelectedTiles;
};
