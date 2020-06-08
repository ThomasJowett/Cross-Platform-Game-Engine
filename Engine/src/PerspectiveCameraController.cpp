#include "stdafx.h"
#include "PerspectiveCameraController.h"

#include "Core/Input.h"
#include "Core/KeyCodes.h"
#include "Core/Settings.h"

PerspectiveCameraController::PerspectiveCameraController(float fovY, Vector3f position, Vector3f forward, Vector3f up)
	:m_AspectRatio((float)(Settings::GetInt("Display", "Screen_Width")) / (float)(Settings::GetInt("Display", "Screen_Height"))),
	m_Camera(fovY, m_AspectRatio, 1.0f, -1.0f), m_FovY(fovY), m_Forward(forward), m_Up(up)
{
	m_NearDepth = 0.0f;
	m_FarDepth = 100.0f;
	SetPosition(position);
	m_Right = Vector3f::Cross(m_Forward, m_Up);
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
		movement.y -= +1.0f;
	}
	
	movement.Normalize();

	movement = movement * m_TranslationSpeed * deltaTime;
	
	Strafe(movement.x);
	Walk(movement.y);
	
	if (Input::IsKeyPressed(KEY_Q))
	{
		Raise(-1.0f * m_TranslationSpeed * deltaTime);
	}
	
	if (Input::IsKeyPressed(KEY_E))
	{
		Raise(+1.0f * m_TranslationSpeed * deltaTime);
	}
	
	if (Input::IsKeyPressed(KEY_J))
	{
		Yaw(-1.0f * deltaTime);
	}
	
	if (Input::IsKeyPressed(KEY_L))
	{
		Yaw(1.0f * deltaTime);
	}
	
	if (Input::IsKeyPressed(KEY_I))
	{
		Pitch(1.0f * deltaTime);
	}
	
	if (Input::IsKeyPressed(KEY_K))
	{
		Pitch(-1.0f * deltaTime);
	}
	
	m_Up = Vector3f::Cross(m_Forward, m_Right).GetNormalized();
	m_Right = Vector3f::Cross(m_Up, m_Forward).GetNormalized();
	
	Matrix4x4 view = Matrix4x4::LookAt(m_CameraPosition, m_CameraPosition + m_Forward, m_Up);
	
	m_Camera.SetPositionAndRotation(m_CameraPosition, m_CameraRotation);
}

void PerspectiveCameraController::OnEvent(Event & event)
{
	EventDispatcher dispatcher(event);
	dispatcher.Dispatch<MouseWheelEvent>(BIND_EVENT_FN(PerspectiveCameraController::OnMouseWheel));
	dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(PerspectiveCameraController::OnWindowResized));
}

bool PerspectiveCameraController::OnMouseWheel(MouseWheelEvent & event)
{
	Walk(event.GetYOffset() * m_TranslationSpeed);
	return false;
}

bool PerspectiveCameraController::OnWindowResized(WindowResizeEvent & event)
{
	SetAspectRatio((float)(event.GetWidth() / event.GetHeight()));
	return false;
}

bool PerspectiveCameraController::OnMouseMoved(MouseMotionEvent & event)
{
	//TODO: rotate the camera on mouse motion
	return false;
}

void PerspectiveCameraController::Walk(float d)
{
	m_CameraPosition = (d * m_Forward) + m_CameraPosition;
}

void PerspectiveCameraController::Strafe(float d)
{
	m_CameraPosition = (d * m_Right) + m_CameraPosition;
}

void PerspectiveCameraController::Raise(float d)
{
	m_CameraPosition = (d * m_Up) + m_CameraPosition;
}

void PerspectiveCameraController::Pitch(float angle)
{
	//TODO: rotate the vectors
	m_CameraRotation = m_CameraRotation + Vector3f(0.0f, angle, 0.0f);
}

void PerspectiveCameraController::Yaw(float angle)
{
	//TODO: rotate the vectors
	m_CameraRotation = m_CameraRotation + Vector3f(0.0f, 0.0f, angle);
}

void PerspectiveCameraController::LookAt(Vector3f focalPoint)
{
	//TODO: rotate vectors to look at a point
}

void PerspectiveCameraController::SetFovY(const float & fovY)
{
	m_FovY = fovY;
	m_Camera.SetProjection(fovY, m_AspectRatio, m_NearDepth, m_FarDepth);
}

void PerspectiveCameraController::SetNearAndFarDepth(const float& nearDepth, const float& farDepth)
{
	m_NearDepth = nearDepth;
	m_FarDepth = farDepth;
	m_Camera.SetProjection(m_FovY, m_AspectRatio, nearDepth, farDepth);
}

void PerspectiveCameraController::SetAspectRatio(const float & aspectRatio)
{
	//TODO: set aspect ratio
}