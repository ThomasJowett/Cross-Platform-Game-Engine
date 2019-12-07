#pragma once

#include "math/Matrix.h"

class OrthographicCamera
{
public:
	OrthographicCamera(float left, float right, float bottom, float top, float nearDepth = -1.0f, float farDepth = 1.0f);

	void SetPosition(const Vector3f& position) {  m_Position = position; RecalculateViewMatrix(); }
	Vector3f GetPosition() { return m_Position; }

	void SetRotation(const Vector3f& rotation) { m_Rotation = rotation; RecalculateViewMatrix(); }
	Vector3f GetRotation() { return m_Rotation; }

	const Matrix4x4 GetProjectionMatrix() const { return m_ProjectionMatrix; }
	const Matrix4x4 GetViewMatrix() const { return m_ViewMatrix; }
	const Matrix4x4 GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }
private:
	void RecalculateViewMatrix();
private:
	Matrix4x4 m_ProjectionMatrix;
	Matrix4x4 m_ViewMatrix;
	Matrix4x4 m_ViewProjectionMatrix;

	Vector3f m_Position;
	Vector3f m_Rotation = { 0.0f, 0.0f, 0.1f };
};