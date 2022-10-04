#pragma once
#include "Core/Layer.h"
#include "Interfaces/ISaveable.h"
#include "Renderer/Tileset.h"

#include <filesystem>

class TilesetView :
	public Layer, public ISaveable
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
	std::string m_WindowName;

	Ref<Tileset> m_Tileset;
	Ref<Tileset> m_LocalTileset;

	float m_Zoom = 1.0f;
	bool m_Dirty = false;
};
