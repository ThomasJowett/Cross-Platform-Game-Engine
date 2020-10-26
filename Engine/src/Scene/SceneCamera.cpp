#include "stdafx.h"
#include "SceneCamera.h"

SceneCamera::SceneCamera()
{
	RecalculatePerspectiveProjection();
}

void SceneCamera::SetOrthoGraphic(float size, float nearClip, float farClip)
{
	m_ProjectionType = ProjectionType::orthographic;
	m_OrthographicSize = size;
	m_OrthoGraphicNear = nearClip;
	m_OrthoGraphicFar = farClip;

	RecalculateOrthographicProjection();
}

void SceneCamera::SetPerspective(float fov, float nearClip, float farClip)
{
	m_ProjectionType = ProjectionType::perspective;
	m_PerspectiveNear = nearClip;
	m_PerspectiveFar = farClip;
	m_Fov = fov;
	RecalculatePerspectiveProjection();
}

void SceneCamera::SetProjection(ProjectionType projectionType)
{
	m_ProjectionType = projectionType;
	RecalculateProjection();
}

void SceneCamera::SetAspectRatio(const float& aspectRatio)
{
	m_AspectRatio = aspectRatio;
	RecalculateProjection();
}

void SceneCamera::RecalculateProjection()
{
	switch (m_ProjectionType)
	{
	case SceneCamera::ProjectionType::perspective:
		RecalculatePerspectiveProjection();
		break;
	case SceneCamera::ProjectionType::orthographic:
		RecalculateOrthographicProjection();
		break;
	}
}

void SceneCamera::RecalculateOrthographicProjection()
{
	float left = -m_OrthographicSize * m_AspectRatio * 0.5f;
	float right = m_OrthographicSize * m_AspectRatio * 0.5f;
	float bottom = -m_OrthographicSize  * 0.5f;
	float top = m_OrthographicSize * 0.5f;
	SetProjectionMatrix(Matrix4x4::OrthographicRH(left, right, bottom, top, m_OrthoGraphicNear, m_OrthoGraphicFar));
}

void SceneCamera::RecalculatePerspectiveProjection()
{
	SetProjectionMatrix(Matrix4x4::PerspectiveRH(m_Fov, m_AspectRatio, m_PerspectiveNear, m_PerspectiveFar));
}
