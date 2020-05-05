#pragma once
#include <include.h>

class GridLayer :
	public Layer
{
public:
	GridLayer();
	~GridLayer() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	virtual void OnUpdate(float deltaTime) override;
	virtual void OnEvent(Event& e) override;
	virtual void OnImGuiRender() override;

private:

	ShaderLibrary m_ShaderLibrary;
	OrthogrpahicCameraController m_CameraController;

	Ref<VertexArray> m_CubeVertexArray;

	Ref<Texture2D> m_Texture;

	float m_Position[3] = { 0.0f, 0.0f, 0.0f };

	Ref<VertexArray> m_GridVertexArray;
};

