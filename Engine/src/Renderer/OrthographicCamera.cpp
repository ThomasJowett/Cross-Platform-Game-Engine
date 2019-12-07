#include "stdafx.h"
#include "OrthographicCamera.h"

OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top, float nearDepth, float farDepth)
	:m_ProjectionMatrix(Matrix4x4::Orthographic(left, right, bottom, top, nearDepth, farDepth)), m_ViewMatrix(Matrix4x4())
{
	m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
}

void OrthographicCamera::RecalculateViewMatrix()
{
	Matrix4x4 transform = Matrix4x4::Translate(-m_Position) * Matrix4x4::Rotate(Quaternion(-m_Rotation));
	m_ViewMatrix = transform; // may have to invert this
	m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
}
