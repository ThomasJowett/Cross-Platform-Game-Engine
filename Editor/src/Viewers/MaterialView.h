#pragma once

#include "Engine.h"
#include "Interfaces/ISaveable.h"

class MaterialView :
	public Layer, public ISaveable
{
public:
	MaterialView(bool* show, std::filesystem::path filepath);
	~MaterialView() = default;

	virtual void OnAttach() override;
	virtual void OnImGuiRender() override;
	virtual void OnUpdate(float deltaTime) override;
	virtual void OnFixedUpdate() override;
private:
	bool* m_Show;

	std::filesystem::path m_FilePath;

	std::string m_WindowName;

	Ref<Material> m_Material;

	bool m_WindowHovered = false;
	bool m_WindowFocussed = false;

	bool m_Dirty = false;

	// Inherited via ISaveable
	virtual void Save() override;

	virtual void SaveAs() override;
};