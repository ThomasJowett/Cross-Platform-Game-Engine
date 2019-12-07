#pragma once

#include "math/Matrix.h"

class Camera
{
public:
	Camera(Matrix4x4 ProjectionMatrix, Matrix4x4 viewMatrix);
	virtual ~Camera() = default;

	void SetPosition(const Vector3f& position) { m_Position = position; RecalculateViewMatrix(); }
	Vector3f GetPosition() { return m_Position; }

	void SetRotation(const Vector3f& rotation) { m_Rotation = rotation; RecalculateViewMatrix(); }
	Vector3f GetRotation() { return m_Rotation; }

	const Matrix4x4 GetProjectionMatrix() const { return m_ProjectionMatrix; }
	const Matrix4x4 GetViewMatrix() const { return m_ViewMatrix; }
	const Matrix4x4 GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }

	float GetNearDepth() const { return m_NearDepth; }
	float GetFarDepth() const { return m_FarDepth; }
protected:
	void RecalculateViewMatrix();
protected:
	Matrix4x4 m_ProjectionMatrix;
	Matrix4x4 m_ViewMatrix;
	Matrix4x4 m_ViewProjectionMatrix;

	Vector3f m_Position;
	Vector3f m_Rotation = { 0.0f, 0.0f, 0.0f };

	float m_NearDepth;
	float m_FarDepth;
};

class OrthographicCamera : public Camera
{
public:
	OrthographicCamera(float left, float right, float bottom, float top, float nearDepth = -1.0f, float farDepth = 1.0f)
		:Camera(Matrix4x4::Orthographic(left, right, bottom, top, nearDepth, farDepth), Matrix4x4())
	{
		m_NearDepth = nearDepth;
		m_FarDepth = farDepth;
	}

	void SetRotation(const float& rotation) { m_Rotation.z = rotation; }
};

class PerspectiveCamera : public Camera
{
public:
	PerspectiveCamera(float fovY, float aspectRatio, float nearDepth = 0.0f, float farDepth = 100.0f)
		:Camera(Matrix4x4::Perspective(fovY, aspectRatio, nearDepth, farDepth), Matrix4x4())
	{
		m_NearDepth = nearDepth;
		m_FarDepth = farDepth;
	}
};