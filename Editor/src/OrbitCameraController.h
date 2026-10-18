#pragma once

#include "Renderer/Camera.h"
#include "Core/BoundingBox.h"
#include "math/Vector2f.h"

// Perspective camera that orbits a target point, pitch is clamped so the view stays upright
class OrbitCameraController
{
public:
	OrbitCameraController();

	void SetViewportSize(float width, float height);

	// Centre on the bounds and pull back far enough to fit them in view
	void Frame(const BoundingBox& bounds);

	void Orbit(const Vector2f& mouseDelta);
	void Pan(const Vector2f& mouseDelta);
	void Zoom(float wheelDelta);

	void SetView(float pitch, float yaw) { m_Pitch = pitch; m_Yaw = yaw; }

	Matrix4x4 GetTransformMatrix() const;
	const Matrix4x4& GetProjectionMatrix() const { return m_Camera.GetProjectionMatrix(); }

	Vector3f GetPosition() const;
	const Vector3f& GetTarget() const { return m_Target; }
	float GetDistance() const { return m_Distance; }

private:
	Vector3f GetForward() const;
	void UpdateProjection();

private:
	PerspectiveCamera m_Camera;

	Vector3f m_Target;
	float m_Distance = 5.0f;
	float m_Pitch = -0.4f;
	float m_Yaw = 0.6f;

	// Size of whatever was last framed, used to scale the zoom limit
	float m_FramedRadius = 1.0f;

	float m_FovY = (float)PI / 4.0f;
	float m_AspectRatio = 4.0f / 3.0f;
	float m_ViewportHeight = 480.0f;

	float m_Sensitivity = 0.005f;
};
