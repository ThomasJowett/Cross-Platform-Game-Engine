#pragma once

#include "Renderer/Camera.h"
#include "Events/ApplicationEvent.h"
#include "Events/MouseEvent.h"

class OrthographicCameraController
{
public:
	OrthographicCameraController();
	~OrthographicCameraController();

	void OnUpdate(float deltaTime);
	void OnEvent(Event& event);

	void SetPosition(const Vector3f& position) { m_Position = position; }
	Vector3f GetPosition() { return m_Position; }

	void SetRotation(const float& rotation) { m_Rotation = rotation; }
	float GetRotation() { return m_Rotation; }

	const OrthographicCamera& GetCamera() { return m_Camera; }

	Matrix4x4 GetTransformMatrix();

	float GetZoom() const { return m_ZoomLevel; }
	void SetZoom(float zoom) {
		m_ZoomLevel = std::clamp(zoom, 0.25f, 1000.0f);
		m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
	}
private:
	bool OnMouseMotion(MouseMotionEvent& event);
	bool OnMouseWheel(MouseWheelEvent& event);
	bool OnWindowResized(WindowResizeEvent& event);

	Vector2f GetScreenSize();
private:
	float  m_AspectRatio;
	float  m_ZoomLevel = 2.0f;
	float m_TranslationSpeed = 1.0f;
	OrthographicCamera m_Camera;

	Vector3f m_Position;
	float m_Rotation = 0.0f;

	bool m_MoveCamera;
	Vector2f m_MouseLastPosition;
};

