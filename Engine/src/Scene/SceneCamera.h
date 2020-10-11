#pragma once

#include "Renderer/Camera.h"

#include "cereal/cereal.hpp"

class SceneCamera :
	public Camera
{
public:
	enum class ProjectionType { perspective = 0, orthographic = 1 };
public:
	SceneCamera();
	virtual ~SceneCamera() = default;

	void SetOrthoGraphic(float size, float nearClip, float farClip);
	void SetPerspective(float fov, float nearClip, float farClip);
	void SetAspectRatio(const float& aspectRatio);

	ProjectionType GetProjectionType() const { return m_ProjectionType; }
	void SetProjection(ProjectionType projectionType);

	float GetOrthoNear() const { return m_OrthoGraphicNear; }
	void SetOrthoNear(float nearClip) { m_OrthoGraphicNear = nearClip; }
	float GetOrthoFar() const { return m_OrthoGraphicFar; }
	void SetOrthoFar(float farClip) { m_OrthoGraphicFar = farClip; }
	float GetOrthoSize() const { return m_OrthographicSize; }
	void SetOrthoSize(float size) { m_OrthographicSize = size; }

	float GetPerspectiveNear() const { return m_PerspectiveNear; }
	void SetPerspectiveNear(float nearClip) { m_PerspectiveNear = nearClip; }
	float GetPerspectiveFar() const { return m_PerspectiveFar; }
	void SetPerspectiveFar(float farClip) { m_PerspectiveFar = farClip; }
	float GetVerticalFov() const { return m_Fov; }
	void SetVerticalFov(float fovY) { m_Fov = fovY; }

	void RecalculateProjection();

	template<typename Archive>
	void serialize(Archive& archive)
	{
		archive(cereal::make_nvp("Projection", m_ProjectionMatrix));
		archive(cereal::make_nvp("ProjectionType", m_ProjectionType));
		archive(cereal::make_nvp("OrthoSize", m_OrthographicSize));
		archive(cereal::make_nvp("OrthoNear", m_OrthoGraphicNear));
		archive(cereal::make_nvp("OrthoFar", m_OrthoGraphicFar));
		archive(cereal::make_nvp("OrthoFar", m_PerspectiveNear));
		archive(cereal::make_nvp("OrthoFar", m_PerspectiveFar));
		archive(cereal::make_nvp("OrthoFar", m_Fov));
		archive(cereal::make_nvp("OrthoFar", m_AspectRatio));
	}

private:
	void RecalculateOrthographicProjection();
	void RecalculatePerspectiveProjection();
private:
	ProjectionType m_ProjectionType = ProjectionType::perspective;
	float m_OrthographicSize = 10.0f;
	float m_OrthoGraphicNear = 1.0f;
	float m_OrthoGraphicFar = -1.0f;

	float m_PerspectiveNear = 1.0f;
	float m_PerspectiveFar = -1.0f;
	float m_Fov = (float)PI * 0.5f;

	float m_AspectRatio = 16.0f / 9.0f;
};