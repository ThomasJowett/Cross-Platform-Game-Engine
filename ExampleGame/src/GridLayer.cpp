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

	m_EarthTexture = Texture2D::Create("resources/Earth.jpg");

	GeneratePositions();
	m_TargetLocation = TargetLocation(m_Time);
	GenerateLaserVertices();

	//Vector3f inputVector(0.523599f,0.785398f,1.5708f);
	Vector3f inputVector(0.5708f, 0.785398f, 0.523599f);
	Quaternion quat(inputVector);

	Vector3f returnVector = quat.EulerAngles();

	std::cout << "input: " << inputVector << std::endl;
	std::cout << "output: " << returnVector << std::endl;
}

void GridLayer::OnDetach()
{
}

void GridLayer::OnFixedUpdate()
{
}

void GridLayer::OnUpdate(float deltaTime)
{
	if (m_Simulate)
	{
		m_Time += deltaTime;
		if (m_Time > 30) m_Time = 0;
	}

	m_TargetLocation = TargetLocation(m_Time);
	GenerateLaserVertices();

	Ref<Shader> shader = m_ShaderLibrary.Get("NormalMap");
	shader->Bind();

	shader->SetFloat4("u_colour", Colours::WHITE);
	shader->SetFloat("u_tilingFactor", 1.0f);

	m_CameraController.OnUpdate(deltaTime);

	Renderer::BeginScene(m_CameraController.GetTransformMatrix(), m_CameraController.GetCamera().GetProjectionMatrix());
	m_Texture->Bind();

	Renderer::Submit(shader, m_CubeVertexArray, Matrix4x4::Translate(Vector3f()));

	Renderer::Submit(shader, m_TargetVertexArray, Matrix4x4::Translate(m_TargetLocation) * Matrix4x4::Rotate(m_PlaneRotation));

	m_EarthTexture->Bind();
	Renderer::Submit(shader, m_EarthVertexArray, Matrix4x4::Translate(Vector3f(0.0f, -6371.0f, 0.0f)) * Matrix4x4::RotateZ(0.57f) * Matrix4x4::RotateY(0.1305f));

	m_Texture->Bind();
	shader->SetFloat4("u_colour", Colours::RED);
	Renderer::Submit(shader, m_IncidentLaserVertexArray, m_IncidentBeamTransform);

	shader->SetFloat4("u_colour", Colours::GREEN);
	Renderer::Submit(shader, m_ReflectionLaserVertexArray, m_ReflectedBeamTransform);

	shader->SetFloat4("u_colour", Colours::YELLOW);
	Renderer::Submit(shader, m_CubeVertexArray, Matrix4x4::Translate(ConvertPolarToCartesian((float)DegToRad(m_Azimuth), (float)DegToRad(m_Elevation), m_Distance)));

	shader->SetFloat4("u_colour", Colours::CYAN);

	for (Vector3f position : m_Positions)
	{
		if (IsPositionIlluminated(position))
		{
			shader->SetFloat4("u_colour", Colours::YELLOW);
		}
		else
		{
			shader->SetFloat4("u_colour", Colours::CYAN);
		}
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
	if (ImGui::Button("Generate Observer Positions"))
		GeneratePositions();
	ImGui::SliderInt("Number of Positions", &m_NumberOfPositions, 0, 1000);
	ImGui::SliderFloat("Minimum distance", &m_MinimumDistance, 0, 1.0);
	ImGui::Text("%s", std::to_string(m_Positions.size()).c_str());
	ImGui::Separator();
	if (ImGui::Button("Recalculate"))
	{
		m_TargetLocation = TargetLocation(m_Time);
		GenerateLaserVertices();
	}
	ImGui::SameLine(); ImGui::Checkbox("Simulate", &m_Simulate);
	ImGui::DragFloat("Time (s)", &m_Time, 1.0f, -30.0f, 30.0f, "%.2f");
	ImGui::InputDouble("Initial Speed (km/s)", &m_TargetInitialSpeed);
	ImGui::InputDouble("Initial Distance (km)", &m_InitialDistance);
	ImGui::InputDouble("Altitude (km)", &m_Altitude);
	ImGui::InputDouble("FlybyAngle (Deg)", &m_FlybyAngle);
	ImGui::InputDouble("Initial Azimuth (Deg)", &m_InitialAzimuth);
	ImGui::DragFloat3("Rotation (Roll) (Yaw) (Pitch)", &m_PlaneRotation[0], 0.01f, -(float)PI, (float)PI);
	ImGui::Text("%s", m_PlaneNormal.to_string().c_str());
	ImGui::Text("%s", m_TargetLocation.to_string().c_str());
	ImGui::Separator();
	ImGui::DragFloat("Azimuth (Deg)", &m_Azimuth, 5.0f, -180.0f, 180.0f);
	ImGui::DragFloat("Elevation (Deg)", &m_Elevation, 5.0f, -180.0f, 180.0f);
	ImGui::DragFloat("Distance (km)", &m_Distance, 0.1f, 0.0f, 10.0f);
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
			float x = Random::FloatInRange((float)LowerXLimit, (float)UpperXLimit);
			float y = Random::FloatInRange((float)LowerYLimit, (float)UpperYLimit) + abs(x) * (float)incline;
			float z = Random::FloatInRange((float)LowerZLimit, (float)UpperZLimit);

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

			float z = Random::FloatInRange((float)LowerZLimit, (float)UpperZLimit);
			float x = Random::FloatInRange((float)LowerXLimit, (float)UpperXLimit);
			float y = Random::FloatInRange((float)LowerYLimit, (float)UpperYLimit) - z * (float)incline;

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
	double flybyAngle = -DegToRad(m_FlybyAngle);
	double distanceTraveled = m_TargetInitialSpeed * time;
	double dl = sqrt((distanceTraveled * distanceTraveled) + (m_InitialDistance * m_InitialDistance)
		- 2.0 * distanceTraveled * m_InitialDistance * cos(flybyAngle));

	double drop = 6371.0 * (1 - cos(dl / 6371.0));
	double dah = m_Altitude - drop;

	double azimuth = asin((distanceTraveled / dl) * sin(flybyAngle)) + DegToRad(m_InitialAzimuth);
	double elevation = (PI * 0.5) - asin(dah / dl);

	m_Azimuth = (float)RadToDeg(azimuth);
	m_Elevation = (float)RadToDeg(elevation);
	m_Distance = (float)dl;

	double horizontalDist = cos(asin(dah / dl)) * dl;
	float z = (float)(cos(DegToRad(m_InitialAzimuth)) * horizontalDist);
	float x = (float)(sin(DegToRad(m_InitialAzimuth)) * horizontalDist);
	float y = (float)dah;

	//return Vector3f(x, y, z);

	return ConvertPolarToCartesian((float)azimuth, (float)elevation, (float)dl);
}

void GridLayer::GenerateLaserVertices()
{
	float distance = m_TargetLocation.Magnitude();

	m_EndDiameter = (float)(sqrt((m_BeamApeture * m_BeamApeture) + (m_Divergence * m_Divergence) * (distance * distance)));

	m_IncidentLaserVertexArray = GeometryGenerator::CreateCylinder((float)m_BeamApeture, m_EndDiameter, distance, 10, 1);

	Vector3f translation = m_TargetLocation - (m_TargetLocation / 2.0f);

	Quaternion planeOrientation(m_PlaneRotation);

	m_PlaneRotation = planeOrientation.EulerAngles();

	Vector3f normal = Vector3f(0.0,0.0, 1.0);

	m_PlaneNormal = planeOrientation.RotateVectorByQuaternion(normal);


	m_IncidentBeamTransform = Matrix4x4::Translate(translation) * Matrix4x4::LookAt(Vector3f(), m_TargetLocation, Vector3f(0.0, 1.0, 0.0)) * Matrix4x4::RotateX(-(float)PI / 2);

	m_ReflectedDiameter = (float)sqrt((m_EndDiameter * m_EndDiameter) + (m_ReflectedDivergence * m_ReflectedDivergence) * (200.0f * 200.0f));

	m_ReflectionLaserVertexArray = GeometryGenerator::CreateCylinder(m_EndDiameter, m_ReflectedDiameter, m_ReflectedLength, 10, 1);

	m_ReflectedDirection = Vector3f::Reflect(m_TargetLocation.GetNormalized(), m_PlaneNormal);

	Vector3f reflectedTranslation = m_TargetLocation + (m_ReflectedDirection * (m_ReflectedLength *0.5f));

	m_ReflectedBeamTransform = Matrix4x4::Translate(reflectedTranslation) * Matrix4x4::LookAt(Vector3f(), m_ReflectedDirection, Vector3f(0, 1.0, 0.0)) * Matrix4x4::RotateX(-(float)PI / 2);
}

// angles in radians
Vector3f GridLayer::ConvertPolarToCartesian(float azimuth, float elevation, float distance)
{
	return Vector3f(distance * sin(elevation) * sin(azimuth), distance * cos(elevation), distance * sin(elevation) * cos(azimuth));
}

bool GridLayer::IsPositionIlluminated(const Vector3f& position)
{
	float coneDistance = Vector3f::Dot(position - m_TargetLocation, m_ReflectedDirection);

	if (coneDistance < 0.0)
		return false;

	double radiusAtObserver = (m_ReflectedDiameter + (m_ReflectedDivergence * m_ReflectedDivergence) * (coneDistance * coneDistance)) / 2.0f;

	double orthDistance = ((position - m_TargetLocation) - coneDistance * m_ReflectedDirection).Magnitude();
	return orthDistance < m_ReflectedDiameter;
}
