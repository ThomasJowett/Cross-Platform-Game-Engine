#include "stdafx.h"
#include "OrthoGrpahicCameraController.h"
#include "Core/Input.h"
#include "Core/KeyCodes.h"
#include "Core/Joysticks.h"
#include "Core/Settings.h"

OrthographicCameraController::OrthographicCameraController()
	:m_AspectRatio((float)(Settings::GetInt("Display", "Screen_Width")) / (float)(Settings::GetInt("Display", "Screen_Height"))),
	m_Camera(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel)
{
}


OrthographicCameraController::~OrthographicCameraController()
{
}

void OrthographicCameraController::OnUpdate(float deltaTime)
{
	PROFILE_FUNCTION();

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

	m_CameraPosition.x += (float)Input::GetJoyStickAxis(0, GAMEPAD_AXIS_LEFT_X) * deltaTime;
	m_CameraPosition.y -= (float)Input::GetJoyStickAxis(0, GAMEPAD_AXIS_LEFT_Y) * deltaTime;

	m_Camera.SetPosition(m_CameraPosition);
}

void OrthographicCameraController::OnEvent(Event & event)
{
	PROFILE_FUNCTION();

	EventDispatcher dispatcher(event);
	dispatcher.Dispatch<MouseWheelEvent>(BIND_EVENT_FN(OrthographicCameraController::OnMouseWheel));
	dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OrthographicCameraController::OnWindowResized));
}

bool OrthographicCameraController::OnMouseWheel(MouseWheelEvent & event)
{
	PROFILE_FUNCTION();

	m_ZoomLevel -= event.GetYOffset() / 4.0f;
	m_ZoomLevel = std::clamp(m_ZoomLevel, 0.25f, 1000.0f);
	m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);

	m_TranslationSpeed = m_ZoomLevel;
	return false;
}

bool OrthographicCameraController::OnWindowResized(WindowResizeEvent & event)
{
	PROFILE_FUNCTION();

	m_AspectRatio = (float)event.GetWidth() / (float)event.GetHeight();
	m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
	return false;
}
