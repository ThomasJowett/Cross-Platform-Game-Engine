#include "FreeCameraControllerScript.h"

CameraController::CameraController()
	:m_CameraComponent(GetComponent<SceneCamera>()),
	m_TransformComponent(GetComponent<TransformComponent>())
{
}

void CameraController::OnUpdate(float deltaTime)
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

	Vector3f forward;

	

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

	//make sure right and forward are orthogonal to each other
	//Vector3f up = Vector3f::Cross(m_Right, m_Forward).GetNormalized();
	//m_Right = Vector3f::Cross(m_Forward, up).GetNormalized();

	m_MouseRelativeVelocity = Vector2f();
}

void CameraController::Walk(float d)
{
	// Calculate the forward direction from the transform
	Quaternion quat(m_TransformComponent.rotation);
	float axis;
	Vector3f angle;
	quat.AxisAngle(axis, angle);
	m_TransformComponent.position = (d * angle) + m_TransformComponent.position;
}

void CameraController::Strafe(float d)
{
}

void CameraController::Raise(float d)
{
}

void CameraController::Pitch(float angle)
{
}

void CameraController::Yaw(float angle)
{
}
