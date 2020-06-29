#pragma once

#include "include.h"

class ViewportCameraController
{
public:
	ViewportCameraController();
	~ViewportCameraController();

	void OnUpdate(float deltaTime);
	void OnEvent(Event& event);

	//void SetPosition(const Vector3f& position) { m_CameraPosition; }

	Camera* GetCamera() const { return m_CurrrentCamera; }
private:
	bool OnMouseWheel(MouseWheelEvent& event);
	bool OnMouseMotion(MouseMotionEvent& event);

	void SwitchCamera();

	void Walk(float d);
	void Strafe(float d);
	void Raise(float d);

	void Pitch(float angle);
	void Yaw(float angle);

private:
	OrthographicCamera m_2DCamera;
	PerspectiveCamera m_3DCamera;

	Vector3f m_3DCameraPosition;
	Vector3f m_2DCameraPosition;

	Camera* m_CurrrentCamera;
	bool m_Is3DCamera;

	float m_AspectRatio;
	float m_ZoomLevel = 2.0f;
	float m_TranslationSpeed = 1.0f;

	float m_FovY = PI / 2;
	float m_NearDepth = 1.0f;
	float m_FarDepth = -1.0f;

	Vector3f m_Up;
	Vector3f m_Right;
	Vector3f m_Forward;
};