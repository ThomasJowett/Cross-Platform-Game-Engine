#pragma once

#include "Renderer/Camera.h"
#include "Events/ApplicationEvent.h"
#include "Events/MouseEvent.h"

class PerspectiveCameraController
{
public:
	PerspectiveCameraController(float fovY = PI * 0.5f,
		Vector3f position = Vector3f(), Vector3f forward = { 0.0f,0.0f, -1.0f }, Vector3f up = { 0.0f, 1.0f, 0.0f });
	~PerspectiveCameraController();

	void OnUpdate(float deltaTime);
	void OnEvent(Event& event);

	void SetPosition(const Vector3f& position) { m_CameraPosition = position; m_Camera.SetPosition(m_CameraPosition); }
	Vector3f GetPosition() { return m_CameraPosition; }

	void SetRotation(const Vector3f& rotation) { m_CameraRotation = rotation; m_Camera.SetRotation(m_CameraRotation); }
	Quaternion GetRotation() { return m_CameraRotation; }

	PerspectiveCamera GetCamera() { return m_Camera; }

	float GetFovY() const { return m_FovY; }
	void SetFovY(const float& fovY);

	void SetNearAndFarDepth(const float& nearDepth, const float& farDepth);

	float GetAspectRatio() const { return m_AspectRatio; }
	void SetAspectRatio(const float& aspectRatio);

	void Walk(float d);
	void Strafe(float d);
	void Raise(float d);

	void Pitch(float angle);
	void Yaw(float angle);

	void LookAt(Vector3f focalPoint);

	Vector3f GetForward() const { return m_Forward; }
	Vector3f GetRight() const { return m_Right; }
	Vector3f GetUp() const { return m_Up; }

	float GetTranslationSpeed() const { return m_TranslationSpeed; }

private:
	bool OnMouseWheel(MouseWheelEvent& event);
	bool OnWindowResized(WindowResizeEvent& event);
	bool OnMouseMoved(MouseMotionEvent& event);
private:
	float m_AspectRatio;
	float m_FovY;
	float m_NearDepth, m_FarDepth;
	float m_TranslationSpeed = 5.0f;
	PerspectiveCamera m_Camera;

	Vector3f m_CameraPosition;
	Vector3f m_CameraRotation;

	Vector3f m_Up;
	Vector3f m_Right;
	Vector3f m_Forward;

	std::pair<double, double> m_LastMousePosition;
	Vector2f m_MouseRelativeVelocity;
	float m_Sensitivity = 0.1f;
};

