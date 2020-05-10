#include "GridLayer.h"
#include "imgui/imgui.h"

GridLayer::GridLayer()
	:Layer("Grid")
{
}

void GridLayer::OnAttach()
{
	m_CubeVertexArray = GeometryGenerator::CreateCube(1.0f, 1.0f, 1.0f);

	m_ShaderLibrary.Load("NormalMap");
	m_Texture = Texture2D::Create("resources/UVChecker.png");
}

void GridLayer::OnDetach()
{
}

void GridLayer::OnUpdate(float deltaTime)
{
	Ref<Shader> shader = m_ShaderLibrary.Get("NormalMap");
	shader->Bind();

	shader->SetInt("u_texture", 0);
	shader->SetFloat4("u_colour", 1.0f, 1.0f, 1.0f, 1.0f);
	shader->SetFloat("u_tilingFactor", 1.0f);

	m_CameraController.OnUpdate(deltaTime);

	Renderer::BeginScene(m_CameraController.GetCamera());
	m_Texture->Bind();

	Renderer::Submit(shader, m_CubeVertexArray, Matrix4x4::Translate({ m_Position[0], m_Position[1], m_Position[2] }));

	Renderer::EndScene();
}

void GridLayer::OnEvent(Event & e)
{
	m_CameraController.OnEvent(e);
}

void GridLayer::OnImGuiRender()
{
	ImGui::Begin("Grid");
	ImGui::Text(m_CameraController.GetCamera().GetPosition().to_string().c_str());
	ImGui::End();
}
