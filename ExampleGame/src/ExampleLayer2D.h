#pragma once
#include <include.h>

class ExampleLayer2D : public Layer
{
public:
	ExampleLayer2D();
	~ExampleLayer2D() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnUpdate(float deltaTime) override;
	virtual void OnEvent(Event& e) override;
	virtual void OnImGuiRender() override;
private:
	OrthogrpahicCameraController m_CameraController;
	Ref<VertexArray> m_VertexArray;
	float m_Colour[4] = { 1.0f,1.0f,1.0f, 1.0f };
	float m_Position[3] = { 0.0f,0.0f,0.0f };
};