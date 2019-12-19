#include "stdafx.h"
#include "OrthoGrpahicCameraController.h"
#include "Core/Input.h"
#include "Core/KeyCodes.h"

OrthogrpahicCameraController::OrthogrpahicCameraController(float aspectRatio)
	:m_AspectRatio(aspectRatio), m_Camera(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel)
{
}


OrthogrpahicCameraController::~OrthogrpahicCameraController()
{
}

void OrthogrpahicCameraController::OnUpdate(float deltaTime)
{
	//moving the camera with the arrow keys

	if (Input::IsKeyPressed(KEY_A))
	{
		m_CameraPosition.x -= m_TranslationSpeed * deltaTime;
	}

	if (Input::IsKeyPressed(KEY_D))
	{
		m_CameraPosition.x += m_TranslationSpeed * deltaTime;
	}

	if (Input::IsKeyPressed(KEY_W))
	{
		m_CameraPosition.y += m_TranslationSpeed * deltaTime;
	}

	if (Input::IsKeyPressed(KEY_S))
	{
		m_CameraPosition.y -= m_TranslationSpeed * deltaTime;
	}

	m_Camera.SetPosition(m_CameraPosition);
}

void OrthogrpahicCameraController::OnEvent(Event & event)
{
	EventDispatcher dispatcher(event);
	dispatcher.Dispatch<MouseWheelEvent>(BIND_EVENT_FN(OrthogrpahicCameraController::OnMouseWheel));
	dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OrthogrpahicCameraController::OnWindowResized));
}

bool OrthogrpahicCameraController::OnMouseWheel(MouseWheelEvent & event)
{
	m_ZoomLevel -= event.GetYOffset() / 4.0f;
	m_ZoomLevel = max(m_ZoomLevel, 0.25f);
	m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);

	m_TranslationSpeed = m_ZoomLevel;
	return false;
}

bool OrthogrpahicCameraController::OnWindowResized(WindowResizeEvent & event)
{
	m_AspectRatio = (float)event.GetWidth() / (float)event.GetHeight();
	m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
	return false;
}
