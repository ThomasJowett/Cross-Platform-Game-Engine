#pragma once

#include "Engine.h"

class ViewportCameraController
{
public:
	ViewportCameraController();
	~ViewportCameraController();

	void OnUpdate(float deltaTime);

	void SetPosition(const Vector3f& position) { m_3DCameraPosition = position; m_2DCameraPosition = position;  m_CurrrentCamera->SetPosition(position); }

	void SetAspectRatio(const float& aspectRatio);

	Camera* GetCamera() const { return m_CurrrentCamera; }

	void OnMouseMotion(Vector2f mousePosition);
	void OnMouseWheel(float mouseWheel);
private:

	void SwitchCamera();

	void Walk(float d);
	void Strafe(float d);
	void Raise(float d);

	void Pitch(float angle);
	void Yaw(float angle);

private:

	Vector3f m_3DCameraPosition;
	Vector3f m_3DCameraRotation;

	Vector3f m_2DCameraPosition;

	Camera* m_CurrrentCamera;
	bool m_Is3DCamera;

	float m_AspectRatio = 16.0f/9.0f;
	float m_ZoomLevel = 2.0f;
	float m_TranslationSpeed = 1.0f;

	float m_FovY = (float)PI / 2;
	float m_NearDepth = 1.0f;
	float m_FarDepth = -1.0f;

	Vector3f m_Up = { 0.0,1.0,0.0 };
	Vector3f m_Right = { 1.0, 0.0, 0.0 };
	Vector3f m_Forward = { 0.0, 0.0, -1.0 };

	Vector2f m_MouseRelativeVelocity;
	Vector2f m_MouseLastPosition;
	float m_Sensitivity = 0.03f;

	OrthographicCamera m_2DCamera;
	PerspectiveCamera m_3DCamera;
};