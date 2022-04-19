#include "stdafx.h"
#include "RigidBody2DComponent.h"

#include "Scene/SceneManager.h"
#include "Box2D/Box2D.h"

RigidBody2DComponent::~RigidBody2DComponent()
{
	if (RuntimeBody)
		SceneManager::CurrentScene()->DestroyBody(RuntimeBody);
}

void RigidBody2DComponent::ApplyImpulse(Vector2f impulse)
{
	RuntimeBody->ApplyLinearImpulse(b2Vec2(impulse.x, impulse.y), RuntimeBody->GetWorldCenter(), true);
}

void RigidBody2DComponent::ApplyImpulseAtPoint(Vector2f impulse, Vector2f center)
{
	RuntimeBody->ApplyLinearImpulse(b2Vec2(impulse.x, impulse.y), b2Vec2(center.x, center.y), true);
}

void RigidBody2DComponent::ApplyForce(Vector2f force)
{
	RuntimeBody->ApplyForce(b2Vec2(force.x, force.y), RuntimeBody->GetWorldCenter(), true);
}

void RigidBody2DComponent::ApplyForceAtPoint(Vector2f force, Vector2f center)
{
	RuntimeBody->ApplyForce(b2Vec2(force.x, force.y), b2Vec2(center.x, center.y), true);
}

void RigidBody2DComponent::ApplyTorque(float torque)
{
	RuntimeBody->ApplyTorque(torque, true);
}

void RigidBody2DComponent::SetLinearVelocity(Vector2f velocity)
{
	RuntimeBody->SetLinearVelocity(b2Vec2(velocity.x, velocity.y));
}

Vector2f RigidBody2DComponent::GetLinearVelocity()
{
	const b2Vec2& vel = RuntimeBody->GetLinearVelocity();
	return Vector2f(vel.x, vel.y);
}
