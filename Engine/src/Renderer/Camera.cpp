#include "stdafx.h"
#include "Camera.h"

Camera::Camera(Matrix4x4 ProjectionMatrix, Matrix4x4 viewMatrix)
	:m_ProjectionMatrix(ProjectionMatrix), m_ViewMatrix(viewMatrix)
{
	m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
}

void Camera::RecalculateViewMatrix()
{
	PROFILE_FUNCTION();

	Matrix4x4 transform = Matrix4x4::Translate(m_Position) * Matrix4x4::Rotate(Quaternion(m_Rotation));
	m_ViewMatrix = Matrix4x4::Inverse(transform);
	m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
}
