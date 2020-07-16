#pragma once
#include <include.h>

class GridLayer :
	public Layer
{
public:
	GridLayer();
	~GridLayer() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	virtual void OnFixedUpdate() override;
	virtual void OnUpdate(float deltaTime) override;
	virtual void OnEvent(Event& e) override;
	virtual void OnImGuiRender() override;

private:
	void GeneratePositions();
	Vector3f TargetLocation(float time);
	void GenerateLaserVertices();

	Vector3f ConvertPolarToCartesian(float azimuth, float elevation, float distance);
private:
	ShaderLibrary m_ShaderLibrary;
	PerspectiveCameraController m_CameraController;

	Ref<VertexArray> m_CubeVertexArray;
	Ref<VertexArray> m_EarthVertexArray;
	Ref<VertexArray> m_TargetVertexArray;
	Ref<VertexArray> m_IncidentLaserVertexArray;
	Ref<VertexArray> m_ReflectionLaserVertexArray;

	Ref<Texture2D> m_Texture;
	Ref<Texture2D> m_EarthTexture;

	float m_Time = 0.0f;

	Vector3f m_TargetLocation;
	double m_TargetInitialSpeed = 0.075;
	double m_InitialDistance = 6.0;
	double m_Altitude = 1.5;
	double m_FlybyAngle = 45.0;

	double m_BeamApeture = 0.0007;
	double m_Divergence = 0.0000677;
	double m_ReflectedDivergence = 0.0025;

	Matrix4x4 m_IncidentBeamTransform;
	Matrix4x4 m_ReflectedBeamTransform;

	Ref<VertexArray> m_GridVertexArray;

	std::vector<Vector3f> m_Positions;
	int m_NumberOfPositions = 20;
	float m_MinimumDistance = 0.01;

	bool m_Simulate = true;

	float m_Azimuth = 0.0f;
	float m_Elevation = 0.0f;
	float m_Distance = 0.0f;
};

