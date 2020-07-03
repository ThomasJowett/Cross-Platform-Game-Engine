#include "GridLayer.h"
#include "imgui/imgui.h"

GridLayer::GridLayer()
	:Layer("Grid")
{
}

void GridLayer::OnAttach()
{
	m_CubeVertexArray = GeometryGenerator::CreateCube(0.05f, 0.1f, 0.05f);

	m_TargetVertexArray = GeometryGenerator::CreateCube(1.0f, 1.0f, 0.001f);

	m_EarthVertexArray = GeometryGenerator::CreateSphere(6371.0f, 300, 200);

	m_ShaderLibrary.Load("NormalMap");

	m_Texture = Texture2D::Create(1, 1);
	uint32_t textureData = Colour(Colours::WHITE).HexValue();

	m_Texture->SetData(&textureData, sizeof(uint32_t));

	m_EarthTexture = Texture2D::Create("resources/Earth.png");

	GeneratePositions();
	m_TargetLocation = TargetLocation(m_Time);
	GenerateLaserVertices();
}

void GridLayer::OnDetach()
{
}

void GridLayer::OnFixedUpdate()
{
	static int count = 0;

	count++;
	if (count > 10)
	{
		count = 0;
		m_Time -= 1;
		if (m_Time < -30) m_Time = 0;
		m_TargetLocation = TargetLocation(m_Time);
		GenerateLaserVertices();
	}
}

void GridLayer::OnUpdate(float deltaTime)
{
	Ref<Shader> shader = m_ShaderLibrary.Get("NormalMap");
	shader->Bind();

	shader->SetFloat4("u_colour", Colours::WHITE);
	shader->SetFloat("u_tilingFactor", 1.0f);

	m_CameraController.OnUpdate(deltaTime);

	Renderer::BeginScene(m_CameraController.GetCamera());
	m_Texture->Bind();

	Renderer::Submit(shader, m_CubeVertexArray, Matrix4x4::Translate(Vector3f()));

	Renderer::Submit(shader, m_TargetVertexArray, Matrix4x4::Translate(m_TargetLocation));

	m_EarthTexture->Bind();
	Renderer::Submit(shader, m_EarthVertexArray, Matrix4x4::Translate(Vector3f(0.0f, -6371.0f, 0.0f)) * Matrix4x4::RotateZ(0.55));

	m_Texture->Bind();
	shader->SetFloat4("u_colour", Colours::RED);
	Renderer::Submit(shader, m_IncidentLaserVertexArray, m_IncidentBeamTransform);

	shader->SetFloat4("u_colour", Colours::GREEN);
	Renderer::Submit(shader, m_ReflectionLaserVertexArray, m_ReflectedBeamTransform);

	shader->SetFloat4("u_colour", Colours::BLUE);

	for (Vector3f position : m_Positions)
	{
		Renderer::Submit(shader, m_CubeVertexArray, Matrix4x4::Translate(position));
	}

	Renderer::EndScene();
}

void GridLayer::OnEvent(Event& e)
{
	m_CameraController.OnEvent(e);
}

void GridLayer::OnImGuiRender()
{
	ImGui::Begin("Settings");
	ImGui::Text(m_CameraController.GetCamera().GetPosition().to_string().c_str());
	if (ImGui::Button("Generate Observer Positions"))
		GeneratePositions();
	ImGui::SliderInt("Number of Positions", &m_NumberOfPositions, 0, 1000);
	ImGui::SliderFloat("Minimum distance", &m_MinimumDistance, 0, 1.0);
	ImGui::Text(std::to_string(m_Positions.size()).c_str());
	ImGui::Separator();
	if (ImGui::Button("Recalculate"))
		m_TargetLocation = TargetLocation(m_Time);
	ImGui::DragFloat("Time", &m_Time, 1.0f, -30.0f, 30.0f, "%.0f");
	ImGui::InputDouble("Initial Speed", &m_TargetInitialSpeed);
	ImGui::InputDouble("Initial Distance", &m_InitialDistance);
	ImGui::InputDouble("Altitude", &m_Altitude);
	ImGui::InputDouble("FlybyAngle", &m_FlybyAngle);
	ImGui::Text(m_TargetLocation.to_string().c_str());
	ImGui::End();
}

void GridLayer::GeneratePositions()
{
	m_Positions.clear();

	double UpperZLimit = -4.0;
	double UpperXLimit = 10.0;
	double UpperYLimit = 0.2;
	double LowerZLimit = -10.0;
	double LowerXLimit = -10.0;
	double LowerYLimit = 0.1;
	double incline = 0.1;

	if (m_MinimumDistance == 0.0f)
	{
		for (size_t i = 0; i < m_NumberOfPositions; i++)
		{
			float x = Random::FloatInRange(LowerXLimit, UpperXLimit);
			float y = Random::FloatInRange(LowerYLimit, UpperYLimit) + abs(x) * incline;
			float z = Random::FloatInRange(LowerZLimit, UpperZLimit);

			m_Positions.push_back(Vector3f(x, y, z));
		}

		return;
	}

	for (size_t i = 0; i < m_NumberOfPositions; i++)
	{
		bool inserted = false;
		int attempts = 0;
		while (!inserted && attempts < 100)
		{
			attempts++;

			float x = Random::FloatInRange(LowerXLimit, UpperXLimit);
			float z = Random::FloatInRange(LowerZLimit, UpperZLimit);
			float y = Random::FloatInRange(LowerYLimit, UpperYLimit) - z * incline;

			bool tooClose = false;
			for (Vector3f otherPosition : m_Positions)
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

Vector3f GridLayer::TargetLocation(float time)
{
	double distanceTraveled = m_TargetInitialSpeed * time;
	double dl = sqrt((distanceTraveled * distanceTraveled) + (m_InitialDistance * m_InitialDistance)
		- 2.0 * distanceTraveled * m_InitialDistance * cos(m_FlybyAngle / 180));

	double drop = 6371.0 * (1 - cos(dl / 6371.0));
	double dah = m_Altitude - drop;

	double azimuth = asin((distanceTraveled / dl) * sin(m_FlybyAngle / 180));
	double elevation = PI * 0.5 - acos(dah / dl);

	float z = (float)(dl * sin(elevation) * cos(azimuth));
	float y = (float)(dl * sin(elevation));
	float x = (float)(dl * cos(elevation) * sin(azimuth));
	return Vector3f(x, y, z);
}

void GridLayer::GenerateLaserVertices()
{
	float distance = m_TargetLocation.Magnitude();

	float endDiameter = sqrt((m_BeamApeture * m_BeamApeture) + (m_Divergence * m_Divergence) * (distance * distance));

	m_IncidentLaserVertexArray = GeometryGenerator::CreateCylinder(m_BeamApeture, endDiameter, distance, 10, 1);

	Vector3f translation = m_TargetLocation - (m_TargetLocation / 2.0f);

	Matrix4x4::LookAt(Vector3f(), Vector3f(0.0,0.0, -1.0), Vector3f(0.0, 1.0, 0.0));

	m_IncidentBeamTransform = Matrix4x4::Translate(translation) * Matrix4x4::LookAt(Vector3f(), m_TargetLocation, Vector3f(0.0, 1.0, 0.0)) * Matrix4x4::RotateX(-PI/2);

	float reflectedDiameter = sqrt((endDiameter * endDiameter) + (m_Divergence * m_Divergence) * (20.0f * 20.0f));

	m_ReflectionLaserVertexArray = GeometryGenerator::CreateCylinder(endDiameter, reflectedDiameter, 10.0f, 10, 1);

	Vector3f reflectedDirection = Vector3f::Reflect(m_TargetLocation.GetNormalized(), Vector3f(0.0, 0.0, 1.0));

	Vector3f reflectedTranslation = m_TargetLocation + (reflectedDirection * 5.0f);

	//m_ReflectedBeamTransform = Matrix4x4::Translate(reflectedTranslation) * /*Matrix4x4::LookAt(reflectedTranslation, reflectedTranslation + reflectedDirection, Vector3f(0, 1.0, 0.0))**/ Matrix4x4::RotateX(-PI / 2);
	m_ReflectedBeamTransform = Matrix4x4::Translate(reflectedTranslation) * Matrix4x4::LookAt(Vector3f(), reflectedDirection, Vector3f(0, 1.0, 0.0)) * Matrix4x4::RotateX(-PI / 2);
}
