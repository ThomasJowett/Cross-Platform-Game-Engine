#include "ViewportCameraController.h"

ViewportCameraController::ViewportCameraController()
	:m_2DCamera(-m_AspectRatio * m_ZoomLevel, m_AspectRatio* m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel),
	m_3DCamera(m_FovY, m_AspectRatio, m_NearDepth, m_FarDepth)
{
	m_Is3DCamera = true;
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

		Yaw(-m_MouseRelativeVelocity.x * deltaTime);
		Pitch(-m_MouseRelativeVelocity.y * deltaTime);


		//make sure right and forward are orthoganal to each other
		Vector3f up = Vector3f::Cross(m_Right, m_Forward).GetNormalized();
		m_Right = Vector3f::Cross(m_Forward, up).GetNormalized();

		m_3DCamera.SetPositionAndRotation(m_3DCameraPosition, m_3DCameraRotation);

		//m_MouseRelativeVelocity = Vector2f();
	}

}

void ViewportCameraController::SetAspectRatio(const float& aspectRatio)
{
	m_AspectRatio = aspectRatio;
	m_3DCamera.SetProjection(m_FovY, m_AspectRatio, m_NearDepth, m_FarDepth);
}

void ViewportCameraController::OnMouseMotion(Vector2f mousePosition)
{
	m_MouseRelativeVelocity = (mousePosition - m_MouseLastPosition) * m_Sensitivity;

	m_MouseLastPosition = mousePosition;
}

void ViewportCameraController::OnMouseWheel(float mouseWheel)
{
	if (Input::IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
	{
		double ln = 10.0 * log(m_TranslationSpeed);
		m_TranslationSpeed = std::clamp(exp(0.1 * (ln + mouseWheel)), (double)FLT_MIN, 1000.0);

		if (m_TranslationSpeed < 0.0f)
			m_TranslationSpeed = FLT_MIN;
	}
	else
	{
		Walk(mouseWheel * m_TranslationSpeed);

		m_3DCamera.SetPosition(m_3DCameraPosition);
	}
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
	m_3DCameraRotation = m_3DCameraRotation + Vector3f(angle, 0.0f, 0.0f);

	if (m_3DCameraRotation.x > PI * 0.5)
		m_3DCameraRotation.x = (PI * 0.5);
	else if (m_3DCameraRotation.x < -(PI * 0.5))
		m_3DCameraRotation.x = -(PI * 0.5);
	else
	{
		Matrix4x4 rotation = Matrix4x4::Rotate(Quaternion(m_Right, angle));
		m_Forward = rotation * m_Forward;
	}
}

void ViewportCameraController::Yaw(float angle)
{
	Matrix4x4 rotation = Matrix4x4::Rotate(Quaternion(m_Up, angle));
	m_Forward = rotation * m_Forward;
	m_Right = rotation * m_Right;

	m_3DCameraRotation = m_3DCameraRotation + Vector3f(0.0f, angle, 0.0f);

	if (m_3DCameraRotation.y > PI)
		m_3DCameraRotation.y -= (PI * 2);
	else if (m_3DCameraRotation.y < -PI)
		m_3DCameraRotation.y += (PI * 2);
}
