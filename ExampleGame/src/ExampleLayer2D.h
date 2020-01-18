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
	Colour m_Colour = Colour::RED;

	int m_ColourName = 0;
	float m_Position[2] = { 0.0f,0.0f };
	float m_Rotation = 0.0f;
	float m_Size[2] = { 1.0f, 1.0f };

	Ref<Texture2D> m_Texture;

	struct ProfileResult
	{
		const char* Name;
		double Time;
	};

	std::vector<ProfileResult> m_ProfileResults;
};