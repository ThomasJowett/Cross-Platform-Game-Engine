#pragma once
#include "Core/Layer.h"
#include "Interfaces/ISaveable.h"
#include "Renderer/Tileset.h"
#include "ViewerManager.h"

#include <filesystem>

class TilesetView :
	public View, public ISaveable
{
public:
	TilesetView(bool* show, std::filesystem::path filepath);
	~TilesetView() = default;

	virtual void OnAttach() override;
	virtual void OnImGuiRender() override;
	// Inherited via ISaveable
	virtual void Save() override;
	virtual void SaveAs() override;
	virtual bool NeedsSaving() override { return m_Dirty; }
private:
	bool* m_Show;

	std::filesystem::path m_Filepath;

	Ref<Tileset> m_Tileset;
	Ref<Tileset> m_LocalTileset;

	float m_Zoom = 1.0f;
	bool m_Dirty = false;

	uint32_t m_SelectedYCoord = 0, m_SelectedXCoord = 0;
};
