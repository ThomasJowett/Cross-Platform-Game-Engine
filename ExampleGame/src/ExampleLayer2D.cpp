#include "ExampleLayer2D.h"
#include "imgui/imgui.h"

ExampleLayer2D::ExampleLayer2D()
	:Layer("Example 2D"),m_CameraController(16.0f/9.0f)
{
}

void ExampleLayer2D::OnAttach()
{
	m_TextureLibrary.Load("resources/UVChecker.png");
}

void ExampleLayer2D::OnDetach()
{
}

void ExampleLayer2D::OnUpdate(float deltaTime)
{
	PROFILE_FUNCTION();
	m_CameraController.OnUpdate(deltaTime);

	Renderer2D::BeginScene(m_CameraController.GetCamera());
	Renderer2D::DrawQuad({ m_Position[0], m_Position[1] }, { m_Size[0], m_Size[1] }, m_Rotation, m_Colour);
	Renderer2D::DrawQuad(Vector2f( m_Position[0] + 1.0f, m_Position[1] ), Vector2f(m_Size[0], m_Size[1] ), m_TextureLibrary.Get("resources/UVChecker.png"),  m_Rotation, m_Colour);
	Renderer2D::DrawQuad(Vector2f(m_Position[0], m_Position[1]+ 1.0f), Vector2f(m_Size[0], m_Size[1]), m_TextureLibrary.Get("resources/UVChecker.png"), m_Rotation);

	Renderer2D::DrawQuad({ m_Position[0] + 1.0f , m_Position[1] + 1.0f }, { m_Size[0], m_Size[1] }, m_TextureLibrary.Get("resources/UVChecker.png"), 1.0f, m_Colour, 10.0f);
	Renderer2D::EndScene();
}

void ExampleLayer2D::OnEvent(Event & e)
{
	m_CameraController.OnEvent(e);
}

void ExampleLayer2D::OnImGuiRender()
{
	ImGui::Begin("Settings 2D");
	ImGui::Text(std::to_string(m_CameraController.GetZoom()).c_str());
	ImGui::ColorEdit4("Square Colour", &m_Colour.r);
	ImGui::DragFloat2("Square Position", m_Position, 0.01f);
	ImGui::DragFloat2("Square Size", m_Size, 0.01f);
	ImGui::DragFloat("Square Rotation", &m_Rotation, 0.001f);

	bool colourClicked = ImGui::Button("Random Colour");

	if (colourClicked)
	{
		m_Colour.SetColour(Colours::RANDOM);
	}
	ImGui::End();
}
