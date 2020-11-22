#include "stdafx.h"
#include "OrthoGrpahicCameraController.h"
#include "Core/Input.h"
#include "Core/KeyCodes.h"
#include "Core/Joysticks.h"
#include "Core/Settings.h"
#include "Core/MouseButtonCodes.h"

OrthographicCameraController::OrthographicCameraController()
	:m_AspectRatio((float)(Settings::GetInt("Display", "Screen_Width")) / (float)(Settings::GetInt("Display", "Screen_Height"))),
	m_Camera(-m_AspectRatio * m_ZoomLevel, m_AspectRatio* m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel),
	m_MoveCamera(false)
{
}


OrthographicCameraController::~OrthographicCameraController()
{
}

void OrthographicCameraController::OnUpdate(float deltaTime)
{
	PROFILE_FUNCTION();

	//moving the camera with the arrow keys

	if (Input::IsKeyPressed(KEY_A) || Input::IsKeyPressed(KEY_LEFT))
	{
		m_Position.x -= m_TranslationSpeed * deltaTime;
	}

	if (Input::IsKeyPressed(KEY_D) || Input::IsKeyPressed(KEY_RIGHT))
	{
		m_Position.x += m_TranslationSpeed * deltaTime;
	}


	if (Input::IsKeyPressed(KEY_W) || Input::IsKeyPressed(KEY_UP))
	{
		m_Position.y += m_TranslationSpeed * deltaTime;
	}

	if (Input::IsKeyPressed(KEY_S) || Input::IsKeyPressed(KEY_DOWN))
	{
		m_Position.y -= m_TranslationSpeed * deltaTime;
	}

	m_Position.x += (float)Input::GetJoyStickAxis(0, GAMEPAD_AXIS_LEFT_X) * deltaTime;
	m_Position.y -= (float)Input::GetJoyStickAxis(0, GAMEPAD_AXIS_LEFT_Y) * deltaTime;
}

void OrthographicCameraController::OnEvent(Event& event)
{
	PROFILE_FUNCTION();

	EventDispatcher dispatcher(event);
	dispatcher.Dispatch<MouseMotionEvent>(BIND_EVENT_FN(OrthographicCameraController::OnMouseMotion));
	dispatcher.Dispatch<MouseWheelEvent>(BIND_EVENT_FN(OrthographicCameraController::OnMouseWheel));
	dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OrthographicCameraController::OnWindowResized));
}

Matrix4x4 OrthographicCameraController::GetTransformMatrix()
{
	return Matrix4x4::Translate(m_Position) * Matrix4x4::RotateZ(m_Rotation);
}

bool OrthographicCameraController::OnMouseMotion(MouseMotionEvent& event)
{
	Vector2f mousePosition(event.GetX(), event.GetY());
	Vector2f mouseRelativePosition = mousePosition - m_MouseLastPosition;

	if (Input::IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE))
	{
		Vector2f screenSize = GetScreenSize();
		m_Position.x -= mouseRelativePosition.x * (m_ZoomLevel / (screenSize.x * 0.5f / m_AspectRatio));
		m_Position.y += mouseRelativePosition.y * (m_ZoomLevel / (screenSize.y * 0.5f ));
	}

	m_MouseLastPosition = mousePosition;
	return false;
}

bool OrthographicCameraController::OnMouseWheel(MouseWheelEvent& event)
{
	PROFILE_FUNCTION();

	m_ZoomLevel -= event.GetYOffset() / 4.0f;
	m_ZoomLevel = std::clamp(m_ZoomLevel, 0.25f, 1000.0f);
	m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);

	m_TranslationSpeed = m_ZoomLevel;
	return false;
}

bool OrthographicCameraController::OnWindowResized(WindowResizeEvent& event)
{
	PROFILE_FUNCTION();

	m_AspectRatio = (float)event.GetWidth() / (float)event.GetHeight();
	m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
	return false;
}

Vector2f OrthographicCameraController::GetScreenSize()
{
	return Vector2f((float)Settings::GetInt("Display", "Screen_Width"), (float)Settings::GetInt("Display", "Screen_Height"));
}