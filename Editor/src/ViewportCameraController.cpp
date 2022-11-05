#include "ViewportCameraController.h"

#include "Engine.h"

ViewportCameraController::ViewportCameraController()
	:m_2DCamera(-m_AspectRatio * m_ZoomLevel, m_AspectRatio* m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel),
	m_3DCamera(m_FovY, m_AspectRatio, m_PerspectiveNearDepth, m_PerspectiveFarDepth)
{
	m_Is3DCamera = false;
	m_CurrentCamera = &m_2DCamera;

	m_2DCameraPosition = Vector3f(0.0f, 0.0f, 0.0f);
	m_3DCameraPosition = Vector3f(0.0f, 0.0f, 0.0f);
}

ViewportCameraController::~ViewportCameraController()
{
}

void ViewportCameraController::OnUpdate(float deltaTime)
{
	PROFILE_FUNCTION();

	auto [mouseX, mouseY] = Input::GetMousePos();
	Vector2f mousePosition(mouseX, mouseY);

	m_MouseRelativeVelocity = (mousePosition - m_MouseLastPosition);

	if (Input::IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE))
	{
		Application::GetWindow().SetCursor(Cursors::ResizeAll);
		if (m_Is3DCamera)
		{
			Strafe(-m_MouseRelativeVelocity.x * 5.0f / (m_ViewPortSize.x * 0.5f / m_AspectRatio));
			Raise(m_MouseRelativeVelocity.y * 5.0f / (m_ViewPortSize.y * 0.5f), m_Up);
		}
		else
		{
			m_2DCameraPosition.x -= m_MouseRelativeVelocity.x * (m_ZoomLevel / (m_ViewPortSize.x * 0.5f / m_AspectRatio));
			m_2DCameraPosition.y += m_MouseRelativeVelocity.y * (m_ZoomLevel / (m_ViewPortSize.y * 0.5f));
		}
	}

	m_MouseLastPosition = mousePosition;

	double mouseWheel = Input::GetMouseWheel();

	if (mouseWheel != 0.0) {
		if (m_Is3DCamera)
		{
			if (Input::IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
			{
				float ln = 10.0f * log(m_TranslationSpeed);
				m_TranslationSpeed = std::clamp((float)exp(0.1f * (ln + mouseWheel)), FLT_MIN, 1000.0f);

				if (m_TranslationSpeed < 0.0f)
					m_TranslationSpeed = FLT_MIN;
			}
			else
			{
				Walk(mouseWheel * m_TranslationSpeed);
			}
		}
		else
		{
			m_ZoomLevel -= mouseWheel / 10.0f * m_ZoomLevel;
			m_ZoomLevel = std::clamp(m_ZoomLevel, 0.25f, 1000.0f);
			m_2DCamera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);

			m_TranslationSpeed = m_ZoomLevel;
		}
	}

	if (m_Is3DCamera && Input::IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
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
			Raise(-1.0f * m_TranslationSpeed * deltaTime, Vector3f(0.0f, 1.0f, 0.0f));
		}

		if (Input::IsKeyPressed(KEY_E))
		{
			Raise(+1.0f * m_TranslationSpeed * deltaTime, Vector3f(0.0f, 1.0f, 0.0f));
		}

		Pitch(-m_MouseRelativeVelocity.y * m_Sensitivity);
		Yaw(-m_MouseRelativeVelocity.x * m_Sensitivity);

		//make sure right and forward are orthogonal to each other
		Vector3f up = Vector3f::Cross(m_Right, m_Forward).GetNormalized();
		m_Right = Vector3f::Cross(m_Forward, up).GetNormalized();
	}
}

Vector3f ViewportCameraController::GetPosition() const
{
	if (m_Is3DCamera)
		return m_3DCameraPosition;
	else
		return m_2DCameraPosition;
}

void ViewportCameraController::SetAspectRatio(const float& aspectRatio)
{
	m_AspectRatio = aspectRatio;
	m_3DCamera.SetProjection(m_FovY, m_AspectRatio, m_PerspectiveNearDepth, m_PerspectiveFarDepth);

	m_2DCamera.SetProjection(-m_ZoomLevel * m_AspectRatio, m_ZoomLevel * m_AspectRatio, -m_ZoomLevel, m_ZoomLevel);
}

Matrix4x4 ViewportCameraController::GetTransformMatrix()
{
	if (m_Is3DCamera)
		return Matrix4x4::Translate(m_3DCameraPosition) * Matrix4x4::Rotate({ m_3DCameraRotation });
	else
		return Matrix4x4::Translate(m_2DCameraPosition);
}

void ViewportCameraController::SwitchCamera(bool is3D)
{
	if (!is3D)
	{
		m_CurrentCamera = &m_2DCamera;
		m_Is3DCamera = false;
	}
	else
	{
		m_CurrentCamera = &m_3DCamera;
		m_Is3DCamera = true;
	}
}

void ViewportCameraController::LookAt(Vector3f focalPoint, float distance)
{
	if (m_Is3DCamera)
		m_3DCameraPosition = focalPoint - (distance * m_Forward);
	else
	{
		m_2DCameraPosition = focalPoint;
		m_2DCameraPosition.z = 0.0f;
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

void ViewportCameraController::Raise(float d, const Vector3f& up)
{
	m_3DCameraPosition = (d * up) + m_3DCameraPosition;
}

void ViewportCameraController::Pitch(float angle)
{
	m_3DCameraRotation.x += angle;

	if (m_3DCameraRotation.x > PI * 0.5)
		m_3DCameraRotation.x = (float)(PI * 0.5);
	else if (m_3DCameraRotation.x < -(PI * 0.5))
		m_3DCameraRotation.x = -(float)(PI * 0.5);
	else
	{
		Matrix4x4 rotation = Matrix4x4::Rotate(Quaternion(m_Right, angle));
		m_Forward = rotation * m_Forward;
		m_Forward.Normalize();

		m_Up = Vector3f::Cross(m_Right, m_Forward);
		m_Up.Normalize();
	}
}

void ViewportCameraController::Yaw(float angle)
{
	m_3DCameraRotation.y += angle;

	if (m_3DCameraRotation.y > PI)
		m_3DCameraRotation.y -= (float)(PI * 2);
	else if (m_3DCameraRotation.y < -PI)
		m_3DCameraRotation.y += (float)(PI * 2);

	Matrix4x4 rotation = Matrix4x4::Rotate(Quaternion(Vector3f(0.0f, 1.0f, 0.0f), angle));
	m_Right = rotation * m_Right;
	m_Right.Normalize();

	m_Forward = Vector3f::Cross(m_Up, m_Right);
	m_Forward.Normalize();

	m_Up = Vector3f::Cross(m_Right, m_Forward);
	m_Up.Normalize();
}