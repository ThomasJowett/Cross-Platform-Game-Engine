#include "stdafx.h"
#include "PerspectiveCameraController.h"

#include "Core/Input.h"
#include "Core/KeyCodes.h"
#include "Core/MouseButtonCodes.h"
#include "Core/Settings.h"
#include "Core/Application.h"

PerspectiveCameraController::PerspectiveCameraController(float fovY, Vector3f position, Vector3f forward, Vector3f up)
	:m_AspectRatio((float)(Settings::GetInt("Display", "Screen_Width")) / (float)(Settings::GetInt("Display", "Screen_Height"))),
	m_Camera(fovY, m_AspectRatio, 1.0f, -1.0f), m_FovY(fovY), m_Forward(forward), m_Up(up)
{
	m_NearDepth = 1.0f;
	m_FarDepth = -1.0f;
	SetPosition(position);
	m_Right = Vector3f::Cross(m_Forward, m_Up);

	m_LastMousePosition = Input::GetMousePos();
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

	if (Input::IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
	{
		Application::GetWindow().DisableCursor();

		Yaw(-m_MouseRelativeVelocity.x * deltaTime);
		Pitch(-m_MouseRelativeVelocity.y * deltaTime);
	}
	else
	{
		Application::GetWindow().EnableCursor();
	}

	if (Input::IsKeyPressed(KEY_J))
	{
		Yaw(1.0f * deltaTime);
	}

	if (Input::IsKeyPressed(KEY_L))
	{
		Yaw(-1.0f * deltaTime);
	}

	if (Input::IsKeyPressed(KEY_I))
	{
		Pitch(1.0f * deltaTime);
	}

	if (Input::IsKeyPressed(KEY_K))
	{
		Pitch(-1.0f * deltaTime);
	}

	//make sure right and forward are orthoganal to each other
	Vector3f up = Vector3f::Cross(m_Right, m_Forward).GetNormalized();
	m_Right = Vector3f::Cross(m_Forward, up).GetNormalized();

	m_Camera.SetPositionAndRotation(m_CameraPosition, m_CameraRotation);

	m_MouseRelativeVelocity = Vector2f();
}

void PerspectiveCameraController::OnEvent(Event& event)
{
	EventDispatcher dispatcher(event);
	dispatcher.Dispatch<MouseWheelEvent>(BIND_EVENT_FN(PerspectiveCameraController::OnMouseWheel));
	dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(PerspectiveCameraController::OnWindowResized));
	dispatcher.Dispatch<MouseMotionEvent>(BIND_EVENT_FN(PerspectiveCameraController::OnMouseMoved));
	dispatcher.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_FN(PerspectiveCameraController::OnMouseButton));
}

bool PerspectiveCameraController::OnMouseButton(MouseButtonPressedEvent& event)
{
	if (event.GetMouseButton() == MOUSE_BUTTON_RIGHT)
	{
		m_MouseRelativeVelocity = Vector2f();
	}
	return false;
}

bool PerspectiveCameraController::OnMouseWheel(MouseWheelEvent& event)
{
	if (Input::IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
	{
		double ln = 10.0 * log(m_TranslationSpeed);
		m_TranslationSpeed = std::clamp(exp(0.1 * (ln + (double)event.GetYOffset())), (double)FLT_MIN, 1000.0);

		if (m_TranslationSpeed < 0.0f)
			m_TranslationSpeed = FLT_MIN;
	}
	else
	{
		Walk(event.GetYOffset() * m_TranslationSpeed);
	}
	return false;
}

bool PerspectiveCameraController::OnWindowResized(WindowResizeEvent& event)
{
	if(event.GetHeight() > 0.0f && event.GetWidth() > 0.0f)
		SetAspectRatio((float)((float)event.GetWidth() / (float)event.GetHeight()));
	return false;
}

bool PerspectiveCameraController::OnMouseMoved(MouseMotionEvent& event)
{
	float xOffset = event.GetX() - m_LastMousePosition.first;
	float yOffset = event.GetY() - m_LastMousePosition.second;

	m_LastMousePosition.first = event.GetX();
	m_LastMousePosition.second = event.GetY();

	m_MouseRelativeVelocity = Vector2f(xOffset, yOffset) * m_Sensitivity;
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
	m_CameraRotation = m_CameraRotation + Vector3f(angle, 0.0f, 0.0f);

	if (m_CameraRotation.x > PI * 0.5)
		m_CameraRotation.x = (PI * 0.5);
	else if (m_CameraRotation.x < -(PI * 0.5))
		m_CameraRotation.x = -(PI * 0.5);
	else
	{
		Matrix4x4 rotation = Matrix4x4::Rotate(Quaternion(m_Right, angle));
		m_Forward = rotation * m_Forward;
	}
}

void PerspectiveCameraController::Yaw(float angle)
{
	Matrix4x4 rotation = Matrix4x4::Rotate(Quaternion(m_Up, angle));
	m_Forward = rotation * m_Forward;
	m_Right = rotation * m_Right;

	m_CameraRotation = m_CameraRotation + Vector3f(0.0f, angle, 0.0f);

	if (m_CameraRotation.y > PI)
		m_CameraRotation.y -= (PI * 2);
	else if (m_CameraRotation.y < -PI)
		m_CameraRotation.y += (PI * 2);
}

void PerspectiveCameraController::LookAt(Vector3f focalPoint)
{
	//TODO: rotate vectors to look at a point
}

void PerspectiveCameraController::SetFovY(const float& fovY)
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

void PerspectiveCameraController::SetAspectRatio(const float& aspectRatio)
{
	m_AspectRatio = aspectRatio;
	m_Camera.SetProjection(m_FovY, m_AspectRatio, m_NearDepth, m_FarDepth);
}