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
	void GeneratePositions();

	ShaderLibrary m_ShaderLibrary;
	PerspectiveCameraController m_CameraController;

	Ref<VertexArray> m_CubeVertexArray;

	Ref<Texture2D> m_Texture;

	float m_Position[3] = { 0.0f, 0.0f, 0.0f };

	Ref<VertexArray> m_GridVertexArray;

	std::vector<Vector3f> m_Positions;
	int m_NumberOfPositions = 20;
	float m_MinimumDistance = 0.01;
};

