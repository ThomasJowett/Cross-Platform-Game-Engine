#pragma once
#include <include.h>

class ExampleLayer3D :public Layer
{
public:
	ExampleLayer3D();
	~ExampleLayer3D() = default;

	void ExampleLayer3D::OnAttach() override;
	void ExampleLayer3D::OnDetach() override;

	void OnUpdate(float deltaTime) override;
	void OnFixedUpdate() override;
	void OnEvent(Event& e) override;
	void OnImGuiRender() override;

private:
	ShaderLibrary m_ShaderLibrary;
	OrthogrpahicCameraController m_CameraController;
	Ref<VertexArray> m_VertexArray;

	Ref<Texture2D> m_Texture;

	float m_Position[3] = { -2.0f, 0.0f, 0.0f };
	float m_Rotation[3] = { 0.0f, 0.0f, 0.0f };
};