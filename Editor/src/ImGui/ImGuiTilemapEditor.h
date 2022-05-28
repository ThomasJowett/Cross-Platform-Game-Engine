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
	void OnRender();

	bool HasSelection();
private:
	DrawMode m_DrawMode = DrawMode::Stamp;

	std::vector<std::vector<bool>> m_SelectedTiles;

	uint32_t m_Rows = 0;
	uint32_t m_Columns = 0;

	std::filesystem::file_time_type m_CurrentFileTime;
};
