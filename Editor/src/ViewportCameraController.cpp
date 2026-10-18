#include "ViewportCameraController.h"

#include "Core/Input.h"
#include "Core/KeyCodes.h"
#include "Core/MouseButtonCodes.h"

#include "Logging/Instrumentor.h"
#include "Core/Application.h"
#include "Core/Cursor.h"
#include "Core/Settings.h"

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

void ViewportCameraController::OnUpdate(float deltaTime, bool hoveredViewport)
{
	PROFILE_FUNCTION();

	auto [mouseX, mouseY] = Input::GetMousePos();
	Vector2f mousePosition((float)mouseX, (float)mouseY);

	m_MouseRelativeVelocity = (mousePosition - m_MouseLastPosition);

	if (Input::IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE) && hoveredViewport)
	{
		Application::GetWindow()->SetCursor(Cursors::ResizeAll);
		if (m_Is3DCamera)
		{
			Strafe(-m_MouseRelativeVelocity.x * 5.0f / (m_ViewPortSize.x * 0.5f / m_AspectRatio));
			Raise(m_MouseRelativeVelocity.y * 5.0f / (m_ViewPortSize.y * 0.5f), m_Up);
		}
		else
		{
			Pan();
		}
	}
	else if (Input::IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && hoveredViewport && !m_Is3DCamera)
	{
		Application::GetWindow()->SetCursor(Cursors::ResizeAll);
		Pan();
	}

	m_MouseLastPosition = mousePosition;

	if (!hoveredViewport)
		return;

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
				Walk((float)mouseWheel * m_TranslationSpeed);
			}
		}
		else
		{
			m_ZoomLevel -= (float)mouseWheel / 10.0f * m_ZoomLevel;
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

const Vector3f& ViewportCameraController::GetPosition() const
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

Matrix4x4 ViewportCameraController::GetTransformMatrix() const
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

void ViewportCameraController::Frame(const BoundingBox& bounds)
{
	Vector3f centre = bounds.Center();
	Vector3f halfSize = (bounds.Max() - bounds.Min()) * 0.5f;

	if (m_Is3DCamera)
	{
		float radius = halfSize.Magnitude();
		if (radius <= 0.0f)
			radius = 0.5f;

		float halfFovY = m_FovY * 0.5f;
		float halfFovX = atanf(tanf(halfFovY) * m_AspectRatio);
		float distance = radius / sinf(std::min(halfFovX, halfFovY));

		// Keep the near plane from cutting into small objects
		distance = std::max(distance, radius + m_PerspectiveNearDepth);

		m_3DCameraPosition = centre - (distance * m_Forward);
	}
	else
	{
		m_2DCameraPosition = Vector3f(centre.x, centre.y, 0.0f);

		float zoomLevel = std::max(halfSize.y, halfSize.x / m_AspectRatio) * 1.1f;
		if (zoomLevel > 0.0f)
		{
			m_ZoomLevel = std::clamp(zoomLevel, 0.25f, 1000.0f);
			m_2DCamera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
			m_TranslationSpeed = m_ZoomLevel;
		}
	}
}

void ViewportCameraController::SetView(float pitch, float yaw)
{
	// No focal point is tracked, so assume it's as far along the view as the world origin is
	float distance = m_3DCameraPosition.Magnitude();
	if (distance < 1.0f)
		distance = 10.0f;
	Vector3f focalPoint = m_3DCameraPosition + (m_Forward * distance);

	m_3DCameraRotation = Vector3f(pitch, yaw, 0.0f);
	Matrix4x4 rotation = Matrix4x4::Rotate(Quaternion(m_3DCameraRotation));
	m_Forward = rotation * Vector3f(0.0f, 0.0f, -1.0f);
	m_Right = rotation * Vector3f(1.0f, 0.0f, 0.0f);
	m_Up = rotation * Vector3f(0.0f, 1.0f, 0.0f);

	m_3DCameraPosition = focalPoint - (m_Forward * distance);
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
void ViewportCameraController::Pan() {
	m_2DCameraPosition.x -= m_MouseRelativeVelocity.x * (m_ZoomLevel / (m_ViewPortSize.x * 0.5f / m_AspectRatio));
	m_2DCameraPosition.y += m_MouseRelativeVelocity.y * (m_ZoomLevel / (m_ViewPortSize.y * 0.5f));
}

/* ------------------------------------------------------------------------------------------------------------------ */

void ViewportCameraController::BeginMouseLook()
{
	// Locking the cursor in a browser shows a "Press Esc" banner every time, so it's optional there
#ifdef __EMSCRIPTEN__
	if (!Settings::GetBool("Editor", "Lock_Cursor_When_Looking"))
	{
		Application::GetWindow()->CaptureMouse();
		return;
	}
#endif
	Application::GetWindow()->DisableCursor(); //TODO: Fix the issue that disabling the cursor jumps the mouse position
}

void ViewportCameraController::EndMouseLook()
{
#ifdef __EMSCRIPTEN__
	if (!Settings::GetBool("Editor", "Lock_Cursor_When_Looking"))
	{
		Application::GetWindow()->ReleaseMouse();
		return;
	}
#endif
	Application::GetWindow()->EnableCursor();
}
