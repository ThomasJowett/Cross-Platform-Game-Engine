#pragma once

#include "Renderer/Camera.h"

class ViewportCameraController
{
public:
	ViewportCameraController();
	~ViewportCameraController();

	void OnUpdate(float deltaTime);

	void SetPosition(const Vector3f& position) { m_3DCameraPosition = position; m_2DCameraPosition = position; }
	Vector3f GetPosition() const;
	const Vector3f GetUp() const { return m_Up; }
	const Vector3f GetForward() const { return m_Forward; }
	const Vector3f GetRight() const { return m_Right; }

	void SetAspectRatio(const float& aspectRatio);
	void SetViewPortSize(const Vector2f& viewportSize) { m_ViewPortSize = viewportSize; }

	Camera* GetCamera() const { return m_CurrentCamera; }

	Matrix4x4 GetTransformMatrix();

	void OnMouseMotion(Vector2f mousePosition);
	void OnMouseWheel(float mouseWheel);
	void SwitchCamera(bool is3D);

	void LookAt(Vector3f focalPoint, float distance);

private:

	void Walk(float d);
	void Strafe(float d);
	void Raise(float d, const Vector3f& up);

	void Pitch(float angle);
	void Yaw(float angle);

private:

	Vector3f m_3DCameraPosition;
	Vector3f m_3DCameraRotation;

	Vector3f m_2DCameraPosition;

	Camera* m_CurrentCamera;
	bool m_Is3DCamera;

	float m_AspectRatio = 16.0f / 9.0f;
	float m_TranslationSpeed = 1.0f;

	float m_FovY = (float)PI / 4;
	float m_PerspectiveNearDepth = 1.0f;
	float m_PerspectiveFarDepth = 1000.0f;

	float m_ZoomLevel = 2.0f;
	float m_OrthographicNearDepth = -1.0f;
	float m_OrthoGraphicFarDepth = 1.0f;

	Vector3f m_Up = { 0.0,1.0,0.0 };
	Vector3f m_Right = { 1.0, 0.0, 0.0 };
	Vector3f m_Forward = { 0.0, 0.0, -1.0 };

	Vector2f m_MouseRelativeVelocity;
	Vector2f m_MouseLastPosition;
	float m_Sensitivity = 0.03f;

	OrthographicCamera m_2DCamera;
	PerspectiveCamera m_3DCamera;

	Vector2f m_ViewPortSize;
};