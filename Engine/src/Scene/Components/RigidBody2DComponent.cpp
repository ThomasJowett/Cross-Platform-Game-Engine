#include "stdafx.h"
#include "RigidBody2DComponent.h"

#include "Scene/SceneManager.h"
#include "box2d/box2d.h"
#include "Scene/Entity.h"
#include "Scene/SceneGraph.h"
#include "Utilities/Triangulation.h"

#include "HierarchyComponent.h"
#include "TransformComponent.h"

void RigidBody2DComponent::ApplyImpulse(Vector2f impulse)
{
	if (runtimeBody)
		runtimeBody->ApplyLinearImpulse(b2Vec2(impulse.x, impulse.y), runtimeBody->GetWorldCenter(), true);
}

void RigidBody2DComponent::ApplyImpulseAtPoint(Vector2f impulse, Vector2f center)
{
	if (runtimeBody)
		runtimeBody->ApplyLinearImpulse(b2Vec2(impulse.x, impulse.y), b2Vec2(center.x, center.y), true);
}

void RigidBody2DComponent::ApplyForce(Vector2f force)
{
	if (runtimeBody)
		runtimeBody->ApplyForce(b2Vec2(force.x, force.y), runtimeBody->GetWorldCenter(), true);
}

void RigidBody2DComponent::ApplyForceAtPoint(Vector2f force, Vector2f center)
{
	if (runtimeBody)
		runtimeBody->ApplyForce(b2Vec2(force.x, force.y), b2Vec2(center.x, center.y), true);
}

void RigidBody2DComponent::ApplyTorque(float torque)
{
	if (runtimeBody)
		runtimeBody->ApplyTorque(torque, true);
}

void RigidBody2DComponent::SetLinearVelocity(Vector2f velocity)
{
	if (runtimeBody)
		runtimeBody->SetLinearVelocity(b2Vec2(velocity.x, velocity.y));
}

Vector2f RigidBody2DComponent::GetLinearVelocity()
{
	const b2Vec2& vel = runtimeBody->GetLinearVelocity();
	return Vector2f(vel.x, vel.y);
}

void RigidBody2DComponent::SetAngularVelocity(float velocity)
{
	if (runtimeBody)
		runtimeBody->SetAngularVelocity(velocity);
}

float RigidBody2DComponent::GetAngularVelocity()
{
	return runtimeBody->GetAngularVelocity();
}

void RigidBody2DComponent::SetTransform(const Vector2f& position, const float& angle)
{
	if (runtimeBody)
	{
		runtimeBody->SetAwake(true);
		runtimeBody->SetTransform(b2Vec2(position.x, position.y), angle);
		runtimeBody->SetLinearVelocity(b2Vec2_zero);
	}
}

void RigidBody2DComponent::GetTransform(Vector2f& position, float& rotation)
{
	if (runtimeBody)
	{
		position = { (float)runtimeBody->GetPosition().x, (float)runtimeBody->GetPosition().y };
		rotation = (float)runtimeBody->GetAngle();
	}
}