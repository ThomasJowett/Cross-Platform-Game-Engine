#pragma once

#include "Renderer/Camera.h"
#include "Core/BoundingBox.h"
#include "math/Quaternion.h"

// Axis aligned views for the view menus, pitch and yaw in radians
struct AxisView
{
	const char* name;
	float pitch;
	float yaw;
};

inline constexpr AxisView s_AxisViews[] = {
	{ "Front",  0.0f,            0.0f },
	{ "Left",   0.0f,            -(float)PIDIV2 },
	{ "Right",  0.0f,            (float)PIDIV2 },
	{ "Top",    -(float)PIDIV2,  0.0f },
	{ "Bottom", (float)PIDIV2,   0.0f },
	{ "Back",   0.0f,            (float)PI }
};

class ViewportCameraController
{
public:
	ViewportCameraController();
	~ViewportCameraController();

	void OnUpdate(float deltaTime, bool hoveredViewport);

	void SetPosition(const Vector3f& position) { m_3DCameraPosition = position; m_2DCameraPosition = position; }
	const Vector3f& GetPosition() const;
	const Vector3f& GetUp() const { return m_Up; }
	const Vector3f& GetForward() const { return m_Forward; }
	const Vector3f& GetRight() const { return m_Right; }

	void SetAspectRatio(const float& aspectRatio);
	void SetViewPortSize(const Vector2f& viewportSize) { m_ViewPortSize = viewportSize; }

	Camera* GetCamera() const { return m_CurrentCamera; }

	Matrix4x4 GetTransformMatrix() const;

	void SwitchCamera(bool is3D);

	// Centre on the bounds and pull back, or zoom out in 2D, until they fit in view
	void Frame(const BoundingBox& bounds);

	// Rotate the 3D camera to face along the given angles, keeping the point it was looking at in view
	void SetView(float pitch, float yaw);

	// Start and stop taking over the mouse to look around; locks the cursor unless the web Editor's preference says not to
	static void BeginMouseLook();
	static void EndMouseLook();

private:

	void Walk(float d);
	void Strafe(float d);
	void Raise(float d, const Vector3f& up);

	void Pitch(float angle);
	void Yaw(float angle);
	void Pan();

private:

	Vector3f m_3DCameraPosition;
	Vector3f m_3DCameraRotation;

	Vector3f m_2DCameraPosition;

	Camera* m_CurrentCamera;
	bool m_Is3DCamera;

	float m_AspectRatio = 16.0f / 9.0f;
	float m_TranslationSpeed = 1.0f;

	float m_FovY = (float)PI / 4;
	float m_PerspectiveNearDepth = 1.0f;
	float m_PerspectiveFarDepth = 1000.0f;

	float m_ZoomLevel = 2.0f;
	float m_OrthographicNearDepth = -1.0f;
	float m_OrthoGraphicFarDepth = 1.0f;

	Vector3f m_Up = { 0.0,1.0,0.0 };
	Vector3f m_Right = { 1.0, 0.0, 0.0 };
	Vector3f m_Forward = { 0.0, 0.0, -1.0 };

	Vector2f m_MouseRelativeVelocity;
	Vector2f m_MouseLastPosition;
	float m_Sensitivity = 0.003f;

	OrthographicCamera m_2DCamera;
	PerspectiveCamera m_3DCamera;

	Vector2f m_ViewPortSize;
};