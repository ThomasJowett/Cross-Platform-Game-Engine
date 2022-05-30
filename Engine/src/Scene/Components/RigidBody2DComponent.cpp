#include "stdafx.h"
#include "RigidBody2DComponent.h"

#include "Scene/SceneManager.h"
#include "Box2D/Box2D.h"
#include "Scene/Entity.h"
#include "Scene/SceneGraph.h"
#include "Utilities/Triangulation.h"

#include "HierarchyComponent.h"
#include "TransformComponent.h"

b2BodyType GetRigidBodyBox2DType(RigidBody2DComponent::BodyType type)
{
	switch (type)
	{
	case RigidBody2DComponent::BodyType::STATIC:
		return b2BodyType::b2_staticBody;
	case RigidBody2DComponent::BodyType::KINEMATIC:
		return b2BodyType::b2_kinematicBody;
	case RigidBody2DComponent::BodyType::DYNAMIC:
		return b2BodyType::b2_dynamicBody;
	}
	return b2BodyType::b2_staticBody;
}

RigidBody2DComponent::~RigidBody2DComponent()
{
	if (runtimeBody)
		SceneManager::CurrentScene()->DestroyBody(runtimeBody);
}

void RigidBody2DComponent::Init(Entity& entity, b2World* b2World)
{
	TransformComponent& transformComp = entity.GetTransform();

	b2BodyDef bodyDef;
	bodyDef.type = GetRigidBodyBox2DType(type);
	bodyDef.position = b2Vec2(transformComp.position.x, transformComp.position.y);
	bodyDef.angle = (float32)transformComp.rotation.z;

	if (type == RigidBody2DComponent::BodyType::DYNAMIC)
	{
		bodyDef.fixedRotation = fixedRotation;
		bodyDef.angularDamping = angularDamping;
		bodyDef.linearDamping = linearDamping;
		bodyDef.gravityScale = gravityScale;
	}

	b2Body* body = b2World->CreateBody(&bodyDef);

	runtimeBody = body;

	if (entity.HasComponent<BoxCollider2DComponent>())
	{
		auto& boxColliderComp = entity.GetComponent<BoxCollider2DComponent>();

		b2PolygonShape boxShape;
		boxShape.SetAsBox(boxColliderComp.Size.x * transformComp.scale.x, boxColliderComp.Size.y * transformComp.scale.y,
			b2Vec2(boxColliderComp.offset.x, boxColliderComp.offset.y),
			0.0f);

		b2FixtureDef fixtureDef;
		fixtureDef.shape = &boxShape;
		fixtureDef.density = boxColliderComp.density;
		fixtureDef.friction = boxColliderComp.friction;
		fixtureDef.restitution = boxColliderComp.restitution;

		body->CreateFixture(&fixtureDef);
	}

	if (entity.HasComponent<CircleCollider2DComponent>())
	{
		auto& circleColliderComp = entity.GetComponent<CircleCollider2DComponent>();

		b2CircleShape circleShape;
		circleShape.m_radius = circleColliderComp.radius * std::max(transformComp.scale.x, transformComp.scale.y);
		circleShape.m_p.Set(circleColliderComp.offset.x, circleColliderComp.offset.y);

		b2FixtureDef fixtureDef;
		fixtureDef.shape = &circleShape;
		fixtureDef.density = circleColliderComp.density;
		fixtureDef.friction = circleColliderComp.friction;
		fixtureDef.restitution = circleColliderComp.restitution;

		body->CreateFixture(&fixtureDef);
	}

	if (entity.HasComponent<PolygonCollider2DComponent>())
	{
		auto& polygonColliderComp = entity.GetComponent<PolygonCollider2DComponent>();

		std::vector<uint32_t> polygonIndices;
		if (Triangulation::Triangulate(polygonColliderComp.vertices, polygonIndices))
		{

			for (size_t i = 0; i < polygonIndices.size(); i += 3)
			{
				b2PolygonShape polygonShape;
				b2Vec2* vertices = new b2Vec2[3];

				for (size_t j = 0; j < 3; j++)
				{
					vertices[j] = b2Vec2(polygonColliderComp.vertices[polygonIndices[i + j]].x * transformComp.scale.x + polygonColliderComp.offset.x,
						polygonColliderComp.vertices[polygonIndices[i + j]].y * transformComp.scale.y + polygonColliderComp.offset.y);
				}
				polygonShape.Set(vertices, 3);

				b2FixtureDef fixtureDef;
				fixtureDef.shape = &polygonShape;
				fixtureDef.density = polygonColliderComp.density;
				fixtureDef.friction = polygonColliderComp.friction;
				fixtureDef.restitution = polygonColliderComp.restitution;

				body->CreateFixture(&fixtureDef);
			}
		}
	}
}

void RigidBody2DComponent::ApplyImpulse(Vector2f impulse)
{
	runtimeBody->ApplyLinearImpulse(b2Vec2(impulse.x, impulse.y), runtimeBody->GetWorldCenter(), true);
}

void RigidBody2DComponent::ApplyImpulseAtPoint(Vector2f impulse, Vector2f center)
{
	runtimeBody->ApplyLinearImpulse(b2Vec2(impulse.x, impulse.y), b2Vec2(center.x, center.y), true);
}

void RigidBody2DComponent::ApplyForce(Vector2f force)
{
	runtimeBody->ApplyForce(b2Vec2(force.x, force.y), runtimeBody->GetWorldCenter(), true);
}

void RigidBody2DComponent::ApplyForceAtPoint(Vector2f force, Vector2f center)
{
	runtimeBody->ApplyForce(b2Vec2(force.x, force.y), b2Vec2(center.x, center.y), true);
}

void RigidBody2DComponent::ApplyTorque(float torque)
{
	runtimeBody->ApplyTorque(torque, true);
}

void RigidBody2DComponent::SetLinearVelocity(Vector2f velocity)
{
	runtimeBody->SetLinearVelocity(b2Vec2(velocity.x, velocity.y));
}

Vector2f RigidBody2DComponent::GetLinearVelocity()
{
	const b2Vec2& vel = runtimeBody->GetLinearVelocity();
	return Vector2f(vel.x, vel.y);
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
