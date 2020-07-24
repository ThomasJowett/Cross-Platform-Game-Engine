#pragma once
#include "include.h"

class ImGuiProperties :
	public Layer
{
public:
	explicit ImGuiProperties(bool* show);
	~ImGuiProperties() = default;

	void OnAttach() override;
	void OnFixedUpdate() override;
	void OnImGuiRender() override;
private:
	bool* m_Show;
};