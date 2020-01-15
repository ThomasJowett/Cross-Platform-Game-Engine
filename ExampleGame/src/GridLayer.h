#pragma once
#include <include.h>
class GridLayer :
	public Layer
{
public:
	GridLayer();
	~GridLayer() = default;

	void GridLayer::OnAttach() override;
	void OnDetach() override;

	void OnUpdate(float deltaTime) override;
	void OnEvent(Event& e) override;
	void OnImGuiRender() override;

private:

	ShaderLibrary m_ShaderLibrary;
	PerspectiveCameraController m_CameraController;
};

