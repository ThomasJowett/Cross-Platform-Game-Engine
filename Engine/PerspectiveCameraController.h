#pragma once

#include "Renderer/Camera.h"
#include "Events/ApplicationEvent.h"
#include "Events/MouseEvent.h"

class PerspectiveCameraController
{
public:
	PerspectiveCameraController(float aspectRatio, float fovY, 
		Vector3f position = Vector3f(), Vector3f forward = { 0.0f,0.0f,-1.0f }, Vector3f up = { 0.0f, 1.0f, 0.0f });
	~PerspectiveCameraController();

	void OnUpdate(float deltaTime);
	void OnEvent(Event& event);

	void SetPosition(const Vector3f& position) { m_CameraPosition = position; m_Camera.SetPosition(m_CameraPosition); }
	Vector3f GetPosition() { return m_CameraPosition; }

	void SetRotation(const Quaternion& rotation) { m_CameraRotation = rotation; m_Camera.SetRotation(m_CameraRotation.EulerAngles()); }
	Quaternion GetRotation() { return m_CameraRotation; }

	PerspectiveCamera GetCamera() { return m_Camera; }

	float GetFovY() const { return m_FovY; }
	void SetFovY(const float& aspectRatio) const;

	float GetAspectRatio() const { return m_AspectRatio; }
	void SetAspectRatio(const float& aspectRatio);

	void Walk(float d);
	void Strafe(float d);
	void Raise(float d);

	void Pitch(float angle);
	void Yaw(float angle);
	
	void LookAt(Vector3f focalPoint);
private:
	bool OnMouseWheel(MouseWheelEvent& event);
	bool OnWindowResized(WindowResizeEvent& event);
	bool OnMouseMoved(MouseMotionEvent& event);
private:
	float m_AspectRatio;
	float m_FovY;
	float m_TranslationSpeed = 1.0f;
	PerspectiveCamera m_Camera;

	Vector3f m_CameraPosition;
	Quaternion m_CameraRotation;

	Vector3f m_Up;
	Vector3f m_Right;
	Vector3f m_Forward;
};

