#include "OrbitCameraController.h"

#include <algorithm>
#include <cmath>

#include "math/Quaternion.h"

// Stop just short of straight up/down so the view never flips over
static constexpr float s_MaxPitch = (float)PIDIV2 - 0.01f;

OrbitCameraController::OrbitCameraController()
	:m_Camera(m_FovY, m_AspectRatio)
{
	UpdateProjection();
}

void OrbitCameraController::SetViewportSize(float width, float height)
{
	if (width <= 0.0f || height <= 0.0f)
		return;

	m_AspectRatio = width / height;
	m_ViewportHeight = height;
	UpdateProjection();
}

void OrbitCameraController::Frame(const BoundingBox& bounds)
{
	if (bounds.IsValid())
	{
		m_Target = bounds.Center();
		m_FramedRadius = (bounds.Max() - bounds.Min()).Magnitude() * 0.5f;
	}
	else
	{
		m_Target = Vector3f();
		m_FramedRadius = 0.0f;
	}

	if (m_FramedRadius <= 0.0f)
		m_FramedRadius = 1.0f;

	float halfFovY = m_FovY * 0.5f;
	float halfFovX = atanf(tanf(halfFovY) * m_AspectRatio);
	m_Distance = m_FramedRadius / sinf(std::min(halfFovX, halfFovY));

	m_Pitch = -0.4f;
	m_Yaw = 0.6f;

	UpdateProjection();
}

void OrbitCameraController::Orbit(const Vector2f& mouseDelta)
{
	m_Yaw -= mouseDelta.x * m_Sensitivity;
	m_Pitch = std::clamp(m_Pitch - mouseDelta.y * m_Sensitivity, -s_MaxPitch, s_MaxPitch);

	m_Yaw = fmodf(m_Yaw, (float)PI * 2.0f);
}

void OrbitCameraController::Pan(const Vector2f& mouseDelta)
{
	// World units covered by one pixel at the target's depth, so the target tracks the cursor
	float unitsPerPixel = 2.0f * m_Distance * tanf(m_FovY * 0.5f) / m_ViewportHeight;

	Matrix4x4 rotation = Matrix4x4::Rotate(Quaternion(Vector3f(m_Pitch, m_Yaw, 0.0f)));
	Vector3f right = rotation * Vector3f(1.0f, 0.0f, 0.0f);
	Vector3f up = rotation * Vector3f(0.0f, 1.0f, 0.0f);

	m_Target = m_Target - (right * mouseDelta.x * unitsPerPixel) + (up * mouseDelta.y * unitsPerPixel);
}

void OrbitCameraController::Zoom(float wheelDelta)
{
	m_Distance *= powf(0.9f, wheelDelta);
	m_Distance = std::clamp(m_Distance, m_FramedRadius * 0.01f, m_FramedRadius * 100.0f);
	UpdateProjection();
}

Matrix4x4 OrbitCameraController::GetTransformMatrix() const
{
	return Matrix4x4::Translate(GetPosition()) * Matrix4x4::Rotate(Quaternion(Vector3f(m_Pitch, m_Yaw, 0.0f)));
}

Vector3f OrbitCameraController::GetPosition() const
{
	return m_Target - (GetForward() * m_Distance);
}

Vector3f OrbitCameraController::GetForward() const
{
	float cosPitch = cosf(m_Pitch);
	return Vector3f(-sinf(m_Yaw) * cosPitch, sinf(m_Pitch), -cosf(m_Yaw) * cosPitch);
}

void OrbitCameraController::UpdateProjection()
{
	// Clip planes follow the zoom so tiny and huge meshes both keep usable depth precision
	float nearDepth = m_Distance * 0.01f;
	float farDepth = (m_Distance + m_FramedRadius) * 100.0f;
	m_Camera.SetProjection(m_FovY, m_AspectRatio, nearDepth, farDepth);
}
