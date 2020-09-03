#include "stdafx.h"
#include "SceneCamera.h"

SceneCamera::SceneCamera()
{
	RecalculatePerspectiveProjection();
}

void SceneCamera::SetOrthoGraphic(float size, float nearClip, float farClip)
{
	IsPerspective = false;
	m_OrthographicSize = size;
	m_OrthoGraphicNear = nearClip;
	m_OrthoGraphicFar = farClip;

	RecalculateOrthographicProjection();
}

void SceneCamera::SetPerspective(float fov, float nearClip, float farClip)
{
	IsPerspective = true;
	m_PerspectiveNear = nearClip;
	m_PerspectiveFar = farClip;
	m_Fov = fov;
	RecalculatePerspectiveProjection();
}

void SceneCamera::SetAspectRatio(const float& aspectRatio)
{
	m_AspectRatio = aspectRatio;
	if (IsPerspective)
		RecalculatePerspectiveProjection();
	else
		RecalculateOrthographicProjection();

	RecalculateViewMatrix();
}

void SceneCamera::RecalculateOrthographicProjection()
{
	float left = -m_OrthographicSize * m_AspectRatio * 0.5f;
	float right = m_OrthographicSize * m_AspectRatio * 0.5f;
	float bottom = m_OrthographicSize * m_AspectRatio * 0.5f;
	float top = -m_OrthographicSize * m_AspectRatio * 0.5f;
	SetProjectionMatrix(Matrix4x4::OrthographicRH(left, right, bottom, top, m_OrthoGraphicNear, m_OrthoGraphicFar));
}

void SceneCamera::RecalculatePerspectiveProjection()
{
	Camera::SetProjectionMatrix(Matrix4x4::PerspectiveRH(m_Fov, m_AspectRatio, m_PerspectiveNear, m_PerspectiveFar));
}
