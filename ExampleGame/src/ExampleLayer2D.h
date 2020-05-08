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
	Ref<VertexArray> m_CubeVertexArray;
	Colour m_Colour = Colours::RED;

	float m_Position[2] = { 0.0f,0.0f };
	float m_Rotation = 0.0f;
	float m_Size[2] = { 1.0f, 1.0f };

	float m_Gridsize = 10.0f;

	TextureLibrary2D m_TextureLibrary;
	Ref<SubTexture2D> m_SubTexture;
};