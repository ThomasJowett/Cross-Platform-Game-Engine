#pragma once
#include "Engine.h"

#include "HeirachyPanel.h"

class PropertiesPanel :
	public Layer
{
public:
	explicit PropertiesPanel(bool* show, HeirachyPanel* heirachyPanel);
	~PropertiesPanel() = default;

	void OnAttach() override;
	void OnFixedUpdate() override;
	void OnImGuiRender() override;
private:
	void DrawComponents(Entity entity);
	void DrawAddComponent(Entity entity);

	template<typename T>
	void AddComponentMenuItem(const char* name, Entity entity)
	{
		if (ImGui::MenuItem(name, nullptr, nullptr, !entity.HasComponent<T>()))
		{
			entity.AddComponent<T>();
		}
	}

private:
	bool* m_Show;

	HeirachyPanel* m_HeirachyPanel;
};