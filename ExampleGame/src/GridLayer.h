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
	OrthogrpahicCameraController m_CameraController;

	Ref<VertexArray> m_CubeVertexArray;

	Ref<Texture2D> m_Texture;

	float m_Position[3] = { 0.0f, 0.0f, 0.0f };

	Ref<VertexArray> m_GridVertexArray;
};

