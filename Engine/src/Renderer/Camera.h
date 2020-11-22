#pragma once

#include "math/Matrix.h"

class Camera
{
public:
	Camera(Matrix4x4 projectionMatrix)
		:m_ProjectionMatrix(projectionMatrix)
	{}
	Camera() = default;
	virtual ~Camera() = default;

	const Matrix4x4& GetProjectionMatrix() const { return m_ProjectionMatrix; }

protected:
	void SetProjectionMatrix(const Matrix4x4& matrix) { m_ProjectionMatrix = matrix; }
protected:
	Matrix4x4 m_ProjectionMatrix;
};

class OrthographicCamera : public Camera
{
	float m_NearDepth;
	float m_FarDepth;

public:
	OrthographicCamera(float left, float right, float bottom, float top, float nearDepth = -1.0f, float farDepth = 1.0f)
		:Camera(Matrix4x4::OrthographicRH(left, right, bottom, top, nearDepth, farDepth))
	{
		m_NearDepth = nearDepth;
		m_FarDepth = farDepth;
	}

	void SetProjection(float left, float right, float bottom, float top, float nearDepth = -1.0f, float farDepth = 1.0f)
	{
		Camera::SetProjectionMatrix(Matrix4x4::OrthographicRH(left, right, bottom, top, nearDepth, farDepth));

		m_NearDepth = nearDepth;
		m_FarDepth = farDepth;
	}
};

class PerspectiveCamera : public Camera
{
	float m_NearDepth;
	float m_FarDepth;

public:
	PerspectiveCamera(float fovY, float aspectRatio, float nearDepth = 1.0f, float farDepth = 1000.0f)
		:Camera(Matrix4x4::PerspectiveRH(fovY, aspectRatio, nearDepth, farDepth))
	{
		m_NearDepth = nearDepth;
		m_FarDepth = farDepth;
	}

	void SetProjection(float fovY, float aspectRatio, float nearDepth = 1.0f, float farDepth = 1000.0f)
	{
		Camera::SetProjectionMatrix(Matrix4x4::PerspectiveRH(fovY, aspectRatio, nearDepth, farDepth));

		m_NearDepth = nearDepth;
		m_FarDepth = farDepth;
	}
};