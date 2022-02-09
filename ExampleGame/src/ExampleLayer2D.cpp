#include "ExampleLayer2D.h"
#include "imgui/imgui.h"

#include "GLFW/include/GLFW/glfw3.h"

ExampleLayer2D::ExampleLayer2D()
	:Layer("Example 2D")
{
}

void ExampleLayer2D::OnAttach()
{
	m_TextureLibrary.Load(Application::Get().GetWorkingDirectory() / "resources" / "UVChecker.png");
	m_SubTexture = CreateRef<SubTexture2D>(m_TextureLibrary.Get("UVChecker.png"), 102, 102 , 0);
}

void ExampleLayer2D::OnDetach()
{
}

void ExampleLayer2D::OnUpdate(float deltaTime)
{
	PROFILE_FUNCTION();
	if (m_ControlCamera)
	{
		m_CameraController.OnUpdate(deltaTime);
	}
	Renderer2D::ResetStats();

	Renderer::BeginScene(m_CameraController.GetTransformMatrix(), m_CameraController.GetCamera().GetProjectionMatrix());

	//A coloured quad in the center of the screen
	Renderer2D::DrawQuad({ m_Position[0], m_Position[1] }, { m_Size[0], m_Size[1] }, m_Rotation, m_Colour);

	//A textured quad tinted
	Renderer2D::DrawQuad(Vector2f(m_Position[0] + 1.0f, m_Position[1]), Vector2f(m_Size[0], m_Size[1]), m_TextureLibrary.Get("UVChecker.png"), m_Rotation, m_Colour);

	//A textured quad
	Renderer2D::DrawQuad(Vector2f(m_Position[0], m_Position[1] + 1.0f), Vector2f(m_Size[0], m_Size[1]), m_TextureLibrary.Get("UVChecker.png"), m_Rotation);

	//A quad textured with a sub texture
	Renderer2D::DrawQuad(Vector2f(m_Position[0], m_Position[1] + 2.0f), Vector2f(m_Size[0]/2, m_Size[1]/2), m_SubTexture, m_Rotation);

	//A textured quad with fixed rotation
	Renderer2D::DrawQuad({ m_Position[0] + 1.0f , m_Position[1] + 1.0f }, { m_Size[0], m_Size[1] }, m_TextureLibrary.Get("UVChecker.png"), 1.0f, m_Colour, 10.0f);

	for (float y = -m_Gridsize; y <= m_Gridsize; y += 0.2f)
	{
		for (float x = -m_Gridsize; x <= m_Gridsize; x += 0.2f)
		{
			Colour colour = { (x + m_Gridsize/2) / m_Gridsize, 0.4f, (y + m_Gridsize/2) / m_Gridsize, 1.0f };
			Renderer2D::DrawQuad({ x,y }, { 0.1f, 0.1f }, x / 3.14f, colour);
		}
	}

	//Renderer2D::DrawLine(Vector2f( -0.3f,-0.4f ), Vector2f( -0.3f, 0.4f ), 1.0f);
	Renderer::EndScene();
}

void ExampleLayer2D::OnEvent(Event& e)
{
	m_CameraController.OnEvent(e);

	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<KeyPressedEvent>([](KeyPressedEvent& event)
		{
			if (event.GetKeyCode() == KEY_F11)
				Application::ToggleImGui();
			return false;
		});
}

void ExampleLayer2D::OnImGuiRender()
{
	ImGui::Begin("Settings 2D");
	ImGui::Checkbox("Control Camera", &m_ControlCamera);
	ImGui::ColorEdit4("Square Colour", &m_Colour.r);
	ImGui::DragFloat2("Square Position", m_Position, 0.01f);
	ImGui::DragFloat2("Square Size", m_Size, 0.01f);
	ImGui::DragFloat("Square Rotation", &m_Rotation, 0.001f);

	bool colourClicked = ImGui::Button("Random Colour");

	if (colourClicked)
	{
		m_Colour.SetColour(Colours::RANDOM);
	}

	Renderer2D::Stats stats = Renderer2D::GetStats();
	ImGui::Text("Renderer Stats: ");
	ImGui::Text("Draw Calls: %d", stats.drawCalls);
	ImGui::Text("Quad Count: %d", stats.quadCount);
	ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
	ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

	ImGui::SliderFloat("Grid Size", &m_Gridsize, 1.0f, 100.0f);
	ImGui::End();
}
