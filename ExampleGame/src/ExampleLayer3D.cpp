#include "ExampleLayer3D.h"
#include "ImGui/imgui.h"

ExampleLayer3D::ExampleLayer3D()
	:Layer("Example"), m_CameraController(16.0f / 9.0f)
{
}

void ExampleLayer3D::OnAttach()
{
	m_CubeVertexArray = GeometryGenerator::CreateCube(1.0f, 1.0f, 1.0f);
	m_SphereVertexArray = GeometryGenerator::CreateSphere(0.5f, 50, 50);
	m_GridVertexArray = GeometryGenerator::CreateGrid(10.0f, 10.0f, 5, 5, 2.0f, 2.0f);

	m_ShaderLibrary.Load("NormalMap");
	m_Texture = Texture2D::Create("resources/UVChecker.png");
}

void ExampleLayer3D::OnDetach()
{
}

void ExampleLayer3D::OnUpdate(float deltaTime)
{
	PROFILE_FUNCTION();

	Ref<Shader> shader = m_ShaderLibrary.Get("NormalMap");
	shader->Bind();

	shader->SetInt("u_texture", 0);
	shader->SetFloat4("u_colour", 1.0f, 1.0f, 1.0f, 1.0f);
	shader->SetFloat("u_tilingFactor", 1.0f);

	m_CameraController.OnUpdate(deltaTime);

	Renderer::BeginScene(m_CameraController.GetCamera());
	m_Texture->Bind();

	Renderer::Submit(shader, m_CubeVertexArray, Matrix4x4::Translate({ m_Position[0], m_Position[1]-1.0f, m_Position[2] }) 
		* Matrix4x4::Rotate(Vector3f(m_Rotation[0], m_Rotation[1], m_Rotation[2])));
	
	Renderer::Submit(shader, m_SphereVertexArray, Matrix4x4::Translate({ m_Position[0], m_Position[1]+1.0f, m_Position[2] })
		* Matrix4x4::Rotate(Vector3f(m_Rotation[0], m_Rotation[1], m_Rotation[2])));
	
	Renderer::Submit(shader, m_GridVertexArray, Matrix4x4::Translate({ m_Position[0] +1.0f, m_Position[1] + 1.0f, m_Position[2] })
		* Matrix4x4::Rotate(Vector3f(m_Rotation[0], m_Rotation[1], m_Rotation[2])));

	Renderer::EndScene();
}

void ExampleLayer3D::OnFixedUpdate()
{
	//m_Rotation[0] += 0.001f;
	//m_Rotation[1] += 0.001f;
	//m_Rotation[2] += 0.001f;
}

void ExampleLayer3D::OnEvent(Event& e)
{
	m_CameraController.OnEvent(e);
}

void ExampleLayer3D::OnImGuiRender()
{
	ImGui::Begin("Settings 3D");
	ImGui::Text(std::to_string(m_CameraController.GetZoom()).c_str());
	ImGui::DragFloat3("Cube Position", m_Position,0.01f);
	ImGui::DragFloat3("Cube Rotation", m_Rotation, 0.001f);
	ImGui::End();
}