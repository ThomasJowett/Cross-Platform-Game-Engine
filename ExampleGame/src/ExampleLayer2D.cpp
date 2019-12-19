#include "ExampleLayer2D.h"
#include "imgui/imgui.h"

ExampleLayer2D::ExampleLayer2D()
	:Layer("Example 2D"),m_CameraController(16.0f/9.0f)
{
}

void ExampleLayer2D::OnAttach()
{
}

void ExampleLayer2D::OnDetach()
{
}

void ExampleLayer2D::OnUpdate(float deltaTime)
{
	m_CameraController.OnUpdate(deltaTime);

	Renderer2D::BeginScene(m_CameraController.GetCamera());
	Renderer2D::DrawQuad({ 1.0f, 0.0f }, { 1.0f, 1.0f }, m_Colour);
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
	ImGui::ColorEdit4("Square Colour", m_Colour);
	ImGui::DragFloat3("Square Position", m_Position, 0.01f);
	ImGui::End();
}
