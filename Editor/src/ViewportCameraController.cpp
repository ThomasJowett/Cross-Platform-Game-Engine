#include "ViewportCameraController.h"

ViewportCameraController::ViewportCameraController()
	:m_2DCamera(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel),
	m_3DCamera(m_FovY, m_AspectRatio, m_NearDepth, m_FarDepth)
{
	m_CurrrentCamera = &m_3DCamera;
}

ViewportCameraController::~ViewportCameraController()
{
}

void ViewportCameraController::OnUpdate(float deltaTime)
{
	PROFILE_FUNCTION();

	if (m_Is3DCamera)
	{
		if (Input::IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
		{
			Vector2f movement;
			if (Input::IsKeyPressed(KEY_A))
			{
				movement.x -= 1.0f;
			}

			if (Input::IsKeyPressed(KEY_D))
			{
				movement.x += 1.0f;
			}

			if (Input::IsKeyPressed(KEY_W))
			{
				movement.y += 1.0f;
			}

			if (Input::IsKeyPressed(KEY_S))
			{
				movement.y -= +1.0f;
			}

			movement.Normalize();

			movement = movement * m_TranslationSpeed * deltaTime;

			Strafe(movement.x);
			Walk(movement.y);
		}
	}

}

void ViewportCameraController::OnEvent(Event& event)
{
	EventDispatcher dispatcher(event);
	dispatcher.Dispatch<MouseWheelEvent>(BIND_EVENT_FN(ViewportCameraController::OnMouseWheel));
	dispatcher.Dispatch<MouseMotionEvent>(BIND_EVENT_FN(ViewportCameraController::OnMouseMotion));
}

bool ViewportCameraController::OnMouseWheel(MouseWheelEvent& event)
{
	return false;
}

bool ViewportCameraController::OnMouseMotion(MouseMotionEvent& event)
{

	return false;
}

void ViewportCameraController::SwitchCamera()
{
	if (m_Is3DCamera)
	{
		m_CurrrentCamera = &m_2DCamera;
		m_Is3DCamera = false;
	}
	else
	{
		m_CurrrentCamera = &m_3DCamera;
		m_Is3DCamera = true;
	}
}

void ViewportCameraController::Walk(float d)
{
	m_3DCameraPosition = (d * m_Forward) + m_3DCameraPosition;
}

void ViewportCameraController::Strafe(float d)
{
	m_3DCameraPosition = (d * m_Right) + m_3DCameraPosition;
}

void ViewportCameraController::Raise(float d)
{
	m_3DCameraPosition = (d * m_Up) + m_3DCameraPosition;
}

void ViewportCameraController::Pitch(float angle)
{
}

void ViewportCameraController::Yaw(float angle)
{
}
