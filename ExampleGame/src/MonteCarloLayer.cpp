#include "MonteCarloLayer.h"

MonteCarloLayer::MonteCarloLayer()
	:Layer("Monte Carlo")
{
	m_Runs = 100;
}

MonteCarloLayer::~MonteCarloLayer()
{
}

void MonteCarloLayer::OnAttach()
{
	m_CameraController.SetPosition({ 0.0f, 0.5f, 0.0f });
	m_CameraController.SetZoom(0.7f);
	RunMonteCarlo();
}

void MonteCarloLayer::OnDetach()
{
}

void MonteCarloLayer::OnUpdate(float deltaTime)
{
	//m_CameraController.OnUpdate(deltaTime);

	Renderer2D::BeginScene(m_CameraController.GetCamera());

	for (Vector2f position : m_BluePositions)
	{
		Renderer2D::DrawQuad(position, Vector2f(0.01f, 0.01f), Colours::BLUE);
	}

	for (Vector2f position : m_RedPositions)
	{
		Renderer2D::DrawQuad(position, Vector2f(0.01f, 0.01f), Colours::RED);
	}
	Renderer2D::EndScene();
}

void MonteCarloLayer::OnFixedUpdate()
{
	m_Count++;

	if (m_Count == 100)
	{
		m_Count = 0;
		m_Runs += 1000;
		RunMonteCarlo();
	}
	if (m_Runs >= 100000)
		m_Runs = 100;
}

void MonteCarloLayer::OnEvent(Event& e)
{
	m_CameraController.OnEvent(e);
}

void MonteCarloLayer::OnImGuiRender()
{
	ImGui::Begin("Monte Carlo Settings");
	ImGui::DragInt("Number of runs", &m_Runs, 1, 0, 100000);
	//if (ImGui::Button("Run Monte Carlo"))
	//	RunMonteCarlo();
	std::string pi = "PI = " + std::to_string((double)(m_RedPositions.size()) / (double)m_BluePositions.size());
	ImGui::Text(pi.c_str());
	ImGui::End();
};
void MonteCarloLayer::RunMonteCarlo()
{
	m_RedPositions.clear();
	m_BluePositions.clear();

	for (size_t i = 0; i < m_Runs; i++)
	{
		float x = Random::Float();
		float y = Random::Float();

		Vector2f position(x, y);

		if (position.SqrMagnitude() < 1.0f)
		{
			m_RedPositions.push_back(position);
		}
		else
		{
			m_BluePositions.push_back(position);
		}
	}
}
