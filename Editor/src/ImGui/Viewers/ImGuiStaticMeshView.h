#pragma once

#include "include.h"

class ImGuiStaticMeshView :
	public Layer
{
	ImGuiStaticMeshView(bool* show);
	~ImGuiStaticMeshView() = default;

	virtual void OnAttach() override;
	virtual void OnImGuiRender() override;
private:
	bool* m_Show;
};

