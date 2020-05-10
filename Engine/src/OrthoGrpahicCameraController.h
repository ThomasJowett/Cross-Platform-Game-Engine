#pragma once

#include "Renderer/Camera.h"
#include "Events/ApplicationEvent.h"
#include "Events/MouseEvent.h"

class OrthogrpahicCameraController
{
public:
	OrthogrpahicCameraController();
	~OrthogrpahicCameraController();

	void OnUpdate(float deltaTime);
	void OnEvent(Event& event);

	void SetPosition(const Vector3f& position) { m_CameraPosition = position; m_Camera.SetPosition(m_CameraPosition); }
	Vector3f GetPosition() { return m_CameraPosition; }

	void SetRotation(const float& rotation) { m_CameraRotation = rotation; m_Camera.SetRotation(Vector3f(0.0f,0.0f, m_CameraRotation)); }
	float GetRotation() { return m_CameraRotation; }

	OrthographicCamera GetCamera() { return m_Camera; }

	float GetZoom() const { return m_ZoomLevel; }
	void SetZoom(float zoom) { m_ZoomLevel = std::clamp(zoom, 0.25f, 1000.0f); }
private:
	bool OnMouseWheel(MouseWheelEvent& event);
	bool OnWindowResized(WindowResizeEvent& event);
private:
	float  m_AspectRatio;
	float  m_ZoomLevel = 2.0f;
	float m_TranslationSpeed = 1.0f;
	OrthographicCamera m_Camera;

	Vector3f m_CameraPosition;
	float m_CameraRotation = 0.0f;
};

