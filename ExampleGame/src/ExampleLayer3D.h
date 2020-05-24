#pragma once
#include <include.h>

class ExampleLayer3D :public Layer
{
public:
	ExampleLayer3D();
	~ExampleLayer3D() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	virtual void OnUpdate(float deltaTime) override;
	virtual void OnFixedUpdate() override;
	virtual void OnEvent(Event& e) override;
	virtual void OnImGuiRender() override;

private:
	ShaderLibrary m_ShaderLibrary;
	OrthogrpahicCameraController m_CameraController;
	Ref<VertexArray> m_CubeVertexArray;
	Ref<VertexArray> m_SphereVertexArray;
	Ref<VertexArray> m_GridVertexArray;
	Ref<VertexArray> m_CylinderVertexArray;
	Ref<VertexArray> m_TorusVertexArray;

	Ref<Texture2D> m_Texture;

	float m_Position[3] = { -2.0f, 0.0f, 0.0f };
	float m_Rotation[3] = { 0.0f, 0.0f, 0.0f };
};