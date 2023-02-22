#pragma once
#include "Scene/Components/TilemapComponent.h"
#include "Scene/Components/TransformComponent.h"
#include "Core/Layer.h"

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
	void SetTilemapComp(const TransformComponent& transformComp, TilemapComponent& tilemapComp);
	bool HasSelection();
	bool IsShown() const { return *m_Show; }
	bool IsHovered() const;

	void Show();
	void Hide();
private:
	bool* m_Show;
	DrawMode m_DrawMode = DrawMode::Stamp;

	std::vector<std::vector<bool>> m_SelectedTiles;

	uint32_t m_Rows = 0;
	uint32_t m_Columns = 0;

	std::filesystem::file_time_type m_CurrentFileTime;

	int m_HoveredCoords[2];

	TilemapComponent* m_TilemapComp = nullptr;
	const TransformComponent* m_TransformComp = nullptr;
};
