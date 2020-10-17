#include "ExampleLayer3D.h"
#include "imgui/imgui.h"

ExampleLayer3D::ExampleLayer3D()
	:Layer("Example 3D")
{
	m_FOV = new float(1.0f);
}

void ExampleLayer3D::OnAttach()
{
	m_CubeVertexArray = GeometryGenerator::CreateCube(1.0f, 1.0f, 1.0f);
	m_SphereVertexArray = GeometryGenerator::CreateSphere(0.5f, 50, 50);
	m_GridVertexArray = GeometryGenerator::CreateGrid(100.0f, 100.0f, 5, 5, 2.0f, 2.0f);
	m_CylinderVertexArray = GeometryGenerator::CreateCylinder(0.25f, 0.1f, 1.0f, 20, 1);
	m_TorusVertexArray = GeometryGenerator::CreateTorus(0.7f, 0.25f, 30);

	m_ShaderLibrary.Load("NormalMap");
	m_Texture = Texture2D::Create("resources/UVChecker.png");

	m_CameraController.SetPosition({ 0.0,0.0,10.0 });
}

void ExampleLayer3D::OnDetach()
{
}

void ExampleLayer3D::OnUpdate(float deltaTime)
{
	PROFILE_FUNCTION();

	Ref<Shader> shader = m_ShaderLibrary.Get("NormalMap");
	shader->Bind();

	shader->SetFloat4("u_colour", Colours::WHITE);
	shader->SetFloat("u_tilingFactor", 1.0f);

	m_CameraController.SetFovY(*m_FOV);
	m_CameraController.SetNearAndFarDepth(m_Nearfar[0], m_Nearfar[1]);

	m_CameraController.OnUpdate(deltaTime);

	Renderer::BeginScene(m_CameraController.GetCamera());
	m_Texture->Bind();

	//Cube
	Renderer::Submit(shader, m_CubeVertexArray, Matrix4x4::Translate({ m_Position[0], m_Position[1] - 1.0f, m_Position[2] })
		* Matrix4x4::Rotate(Vector3f(m_Rotation[0], m_Rotation[1], m_Rotation[2])));

	//Sphere
	Renderer::Submit(shader, m_SphereVertexArray, Matrix4x4::Translate({ m_Position[0], m_Position[1] + 1.0f, m_Position[2] })
		* Matrix4x4::Rotate(Vector3f(m_Rotation[0], m_Rotation[1], m_Rotation[2])));

	//Cylinder
	Renderer::Submit(shader, m_CylinderVertexArray, Matrix4x4::Translate({ m_Position[0] - 2.0f, m_Position[1] + 1.0f, m_Position[2] })
		* Matrix4x4::Rotate(Vector3f(m_Rotation[0], m_Rotation[1], m_Rotation[2])));

	//Grid
	Renderer::Submit(shader, m_GridVertexArray, Matrix4x4::Translate({ m_Position[0], m_Position[1], m_Position[2] - 0.5f })
		* Matrix4x4::Rotate(Vector3f((float)PI / 2.0f, 0.0f, 0.0f)));

	//Torus
	Renderer::Submit(shader, m_TorusVertexArray, Matrix4x4::Translate({ m_Position[0] - 2.0f, m_Position[1] - 1.0f, m_Position[2] })
		* Matrix4x4::Rotate(Vector3f(m_Rotation[0], m_Rotation[1], m_Rotation[2])));

	Renderer::EndScene();
}

void ExampleLayer3D::OnFixedUpdate()
{
	m_Rotation[0] += 0.0011f;
	m_Rotation[1] += 0.0014f;
	m_Rotation[2] += 0.002f;
}

void ExampleLayer3D::OnEvent(Event& e)
{
	m_CameraController.OnEvent(e);
}

void ExampleLayer3D::OnImGuiRender()
{
	ImGui::Begin("Settings 3D");
	ImGui::Text("%s", std::to_string(m_CameraController.GetTranslationSpeed()).c_str());
	ImGui::DragFloat3("Cube Position", m_Position, 0.01f);
	ImGui::DragFloat3("Cube Rotation", m_Rotation, 0.001f);
	ImGui::DragFloat("Fov", m_FOV, 0.01f, 0.0f, (float)PI);
	ImGui::DragFloat2("near far depth", m_Nearfar, 1.0f);
	ImGui::End();
}