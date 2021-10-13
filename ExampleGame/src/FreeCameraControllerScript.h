#pragma once
#include "Engine.h"
class CameraController : public ScriptableEntity
{
	//TODO: implement a camera controller
public:
	CameraController();

	void OnUpdate(float deltaTime) override;

	void Walk(float d);
	void Strafe(float d);
	void Raise(float d);

	void Pitch(float angle);
	void Yaw(float angle);

private:
	float m_TranslationSpeed = 5.0f;

	std::pair<double, double> m_LastMousePosition;
	Vector2f m_MouseRelativeVelocity;
	float m_Sensitivity = 0.1f;

	TransformComponent& m_TransformComponent;
	SceneCamera& m_CameraComponent;
};