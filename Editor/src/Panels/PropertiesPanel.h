#pragma once
#include "include.h"

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
private:
	bool* m_Show;

	HeirachyPanel* m_HeirachyPanel;
};