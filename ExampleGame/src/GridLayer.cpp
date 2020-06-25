#include "GridLayer.h"
#include "imgui/imgui.h"

GridLayer::GridLayer()
	:Layer("Grid")
{
}

void GridLayer::OnAttach()
{
	m_CubeVertexArray = GeometryGenerator::CreateCube(0.05f, 0.05f, 0.1f);

	m_ShaderLibrary.Load("NormalMap");
	m_Texture = Texture2D::Create("resources/UVChecker.png");

	GeneratePositions();

	//m_CameraController.SetRotation({ PI / 2, 0.0f,0.0f });
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

	for (Vector3f position : m_Positions)
	{
		Renderer::Submit(shader, m_CubeVertexArray, Matrix4x4::Translate(position));
	}

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
	if (ImGui::Button("Generate Positions"))
		GeneratePositions();
	ImGui::SliderInt("Number of Positions", &m_NumberOfPositions, 0, 10000);
	ImGui::SliderFloat("Minimum distance", &m_MinimumDistance, 0, 1.0);
	ImGui::Text(std::to_string(m_Positions.size()).c_str());
	ImGui::End();
}

void GridLayer::GeneratePositions()
{
	m_Positions.clear();

	double UpperXLimit = -4.0;
	double UpperYLimit = 10.0;
	double UpperZLimit = 0.2;
	double LowerXLimit = -10.0;
	double LowerYLimit = -10.0;
	double LowerZLimit = 0.1;
	double incline = 0.1;

	if (m_MinimumDistance == 0.0f)
	{
		for (size_t i = 0; i < m_NumberOfPositions; i++)
		{
			float x = Random::FloatInRange(LowerXLimit, UpperXLimit);
			float y = Random::FloatInRange(LowerYLimit, UpperYLimit);
			float z = Random::FloatInRange(LowerZLimit, LowerZLimit) - x * incline;

			m_Positions.push_back(Vector3f(x, y, z));
		}

		return;
	}

	int gridWidth = std::ceil((UpperXLimit - LowerXLimit) / (2.0 * m_MinimumDistance));
	int gridHeight = std::ceil((UpperYLimit - LowerYLimit) / (2.0 * m_MinimumDistance));


	for (size_t i = 0; i < m_NumberOfPositions; i++)
	{
		bool inserted = false;
		int attempts = 0;
		while (!inserted && attempts < 100)
		{
			attempts++;

			float x = Random::FloatInRange(LowerXLimit, UpperXLimit);
			float y = Random::FloatInRange(LowerYLimit, UpperYLimit);
			float z = Random::FloatInRange(LowerZLimit, LowerZLimit) - x * incline;

			bool tooClose = false;
			for (Vector3f otherPosition: m_Positions)
			{
				if (Vector3f::Distance(Vector3f(x, y, z), otherPosition) < m_MinimumDistance)
				{
					tooClose = true;
					break;
				}
			}

			if (!tooClose)
			{
				m_Positions.push_back(Vector3f(x, y, z));
				inserted = true;
			}
		}
	}

	if (m_Positions.size() < m_NumberOfPositions)
	{
		CLIENT_WARN("Number of positions was {0}, could only fit {1}", m_NumberOfPositions, m_Positions.size());
	}
}
