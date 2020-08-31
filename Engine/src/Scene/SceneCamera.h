#pragma once
#include "Renderer/Camera.h"
class SceneCamera :
	public Camera
{
public:
	SceneCamera();
	virtual ~SceneCamera() = default;

	void SetOrthoGraphic(float size, float nearClip, float farClip);
	void SetPerspective(float fov, float nearClip, float farClip);
	void SetAspectRatio(const float& aspectRatio);
private:
	void RecalculateOrthographicProjection();
	void RecalculatePerspectiveProjection();
private:
	float m_OrthographicSize = 10.0f;
	float m_OrthoGraphicNear = 1.0f;
	float m_OrthoGraphicFar = -1.0f;

	float m_PerspectiveNear = 1.0f;
	float m_PerspectiveFar = -1.0f;
	float m_Fov = PI * 0.5f;

	float m_AspectRatio = 16.0f/9.0f;

	bool m_IsPerspective = true;
};