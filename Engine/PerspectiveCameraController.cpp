#include "stdafx.h"
#include "PerspectiveCameraController.h"

#include "Core/Input.h"
#include "Core/KeyCodes.h"

PerspectiveCameraController::PerspectiveCameraController(float aspectRatio, float fovY, Vector3f position, Vector3f forward, Vector3f up)
	:m_Camera(fovY, aspectRatio), m_AspectRatio(aspectRatio), m_FovY(fovY), m_Forward(forward), m_Up(up)
{
	SetPosition(position);
	m_Right = Vector3f::Cross(m_Up, m_Forward);
}

PerspectiveCameraController::~PerspectiveCameraController()
{
}

void PerspectiveCameraController::OnUpdate(float deltaTime)
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
		movement.y - +1.0f;
	}

	movement.Normalize();

	Strafe(movement.x);
	Walk(movement.y);

	if (Input::IsKeyPressed(KEY_Q))
	{
		Raise(1.0f);
	}

	if (Input::IsKeyPressed(KEY_E))
	{
		Raise(-1.0f);
	}

	if (Input::IsKeyPressed(KEY_J))
	{
		Yaw(-1.0f);
	}

	if (Input::IsKeyPressed(KEY_L))
	{
		Yaw(1.0f);
	}

	if (Input::IsKeyPressed(KEY_I))
	{
		Pitch(1.0f);
	}

	if (Input::IsKeyPressed(KEY_K))
	{
		Pitch(-1.0f);
	}
}

void PerspectiveCameraController::OnEvent(Event & event)
{
	EventDispatcher dispatcher(event);
	dispatcher.Dispatch<MouseWheelEvent>(BIND_EVENT_FN(PerspectiveCameraController::OnMouseWheel));
	dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(PerspectiveCameraController::OnWindowResized));
}

bool PerspectiveCameraController::OnMouseWheel(MouseWheelEvent & event)
{
	Walk(event.GetYOffset());
	return false;
}

bool PerspectiveCameraController::OnWindowResized(WindowResizeEvent & event)
{
	SetAspectRatio(event.GetWidth() / event.GetHeight());
	return false;
}

bool PerspectiveCameraController::OnMouseMoved(MouseMotionEvent & event)
{
	//TODO: rotate the camera on mouse motion
	return false;
}

void PerspectiveCameraController::Walk(float d)
{
	SetPosition((d * m_Forward) + m_CameraPosition);
}

void PerspectiveCameraController::Strafe(float d)
{
	SetPosition((d * m_Right) + m_CameraPosition);
}

void PerspectiveCameraController::Raise(float d)
{
	SetPosition((d * m_Up) + m_CameraPosition);
}
