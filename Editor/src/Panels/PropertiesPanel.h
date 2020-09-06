#pragma once
#include "include.h"

class PropertiesPanel :
	public Layer
{
public:
	explicit PropertiesPanel(bool* show);
	~PropertiesPanel() = default;

	void OnAttach() override;
	void OnFixedUpdate() override;
	void OnImGuiRender() override;
private:
	bool* m_Show;
};