#include "stdafx.h"
#include "RigidBody2DComponent.h"

#include "Scene/SceneManager.h"
#include "box2d/box2d.h"
#include "Scene/Entity.h"
#include "Scene/SceneGraph.h"
#include "Utilities/Triangulation.h"

#include "HierarchyComponent.h"
#include "TransformComponent.h"

void SetPhysicsMaterial(b2FixtureDef& fixtureDef, Ref<PhysicsMaterial> physicsMaterial)
{
	if (physicsMaterial)
	{
		fixtureDef.density = physicsMaterial->GetDensity();
		fixtureDef.friction = physicsMaterial->GetFriction();
		fixtureDef.restitution = physicsMaterial->GetRestitution();
	}
	else
	{
		Ref<PhysicsMaterial> defaultPhysicsMaterial = PhysicsMaterial::GetDefaultPhysicsMaterial();
		fixtureDef.density = defaultPhysicsMaterial->GetDensity();
		fixtureDef.friction = defaultPhysicsMaterial->GetFriction();
		fixtureDef.restitution = defaultPhysicsMaterial->GetRestitution();
	}
}

uint32_t RigidBody2DComponent::GetRigidBodyBox2DType(RigidBody2DComponent::BodyType type)
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
	bodyDef.type = (b2BodyType)GetRigidBodyBox2DType(type);
	bodyDef.position = b2Vec2(transformComp.position.x, transformComp.position.y);
	bodyDef.angle = (float)transformComp.rotation.z;

	if (type == RigidBody2DComponent::BodyType::DYNAMIC)
	{
		bodyDef.fixedRotation = fixedRotation;
		bodyDef.angularDamping = angularDamping;
		bodyDef.linearDamping = linearDamping;
		bodyDef.gravityScale = gravityScale;
	}

	b2Body* body = b2World->CreateBody(&bodyDef);

	runtimeBody = body;

	LuaScriptComponent* luaScriptComponent = entity.TryGetComponent<LuaScriptComponent>();

	if (luaScriptComponent && !luaScriptComponent->IsContactListener())
		luaScriptComponent = nullptr;

	if (entity.HasComponent<BoxCollider2DComponent>())
	{
		auto& boxColliderComp = entity.GetComponent<BoxCollider2DComponent>();

		b2PolygonShape boxShape;
		boxShape.SetAsBox(abs(boxColliderComp.size.x * transformComp.scale.x), abs(boxColliderComp.size.y * transformComp.scale.y),
			b2Vec2(boxColliderComp.offset.x, boxColliderComp.offset.y),
			0.0f);

		b2FixtureDef fixtureDef;
		fixtureDef.shape = &boxShape;
		SetPhysicsMaterial(fixtureDef, boxColliderComp.physicsMaterial);
		fixtureDef.userData.pointer = (uintptr_t)entity.GetHandle();

		b2Fixture* fixture = body->CreateFixture(&fixtureDef);

		if (luaScriptComponent)
			luaScriptComponent->m_Fixtures.push_back(fixture);
	}

	if (entity.HasComponent<CircleCollider2DComponent>())
	{
		auto& circleColliderComp = entity.GetComponent<CircleCollider2DComponent>();

		b2CircleShape circleShape;
		circleShape.m_radius = abs(circleColliderComp.radius * std::max(transformComp.scale.x, transformComp.scale.y));
		circleShape.m_p.Set(circleColliderComp.offset.x, circleColliderComp.offset.y);

		b2FixtureDef fixtureDef;
		fixtureDef.shape = &circleShape;
		SetPhysicsMaterial(fixtureDef, circleColliderComp.physicsMaterial);
		fixtureDef.userData.pointer = (uintptr_t)entity.GetHandle();

		b2Fixture* fixture = body->CreateFixture(&fixtureDef);
		if (luaScriptComponent)
			luaScriptComponent->m_Fixtures.push_back(fixture);
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
				SetPhysicsMaterial(fixtureDef, polygonColliderComp.physicsMaterial);

				fixtureDef.userData.pointer = (uintptr_t)entity.GetHandle();

				b2Fixture* fixture = body->CreateFixture(&fixtureDef);
				if (luaScriptComponent)
					luaScriptComponent->m_Fixtures.push_back(fixture);
			}
		}
	}

	if (entity.HasComponent<CapsuleCollider2DComponent>())
	{
		auto& capsuleColliderComp = entity.GetComponent<CapsuleCollider2DComponent>();
		if (capsuleColliderComp.direction == CapsuleCollider2DComponent::Direction::Vertical)
		{
			float scaledHeight = abs(capsuleColliderComp.height * transformComp.scale.y);
			float scaledRadius = abs(capsuleColliderComp.radius * transformComp.scale.x);
			float halfHeight = scaledHeight / 2.0f;
			float diameter = (2.0f * scaledRadius);
			if (scaledHeight < diameter)
				halfHeight = scaledRadius;
			b2CircleShape topShape;
			topShape.m_radius = scaledRadius;
			topShape.m_p.Set(capsuleColliderComp.offset.x, capsuleColliderComp.offset.y + halfHeight - scaledRadius);

			b2FixtureDef topCirclefixtureDef;
			topCirclefixtureDef.userData.pointer = (uintptr_t)entity.GetHandle();
			topCirclefixtureDef.shape = &topShape;

			SetPhysicsMaterial(topCirclefixtureDef, capsuleColliderComp.physicsMaterial);
			body->CreateFixture(&topCirclefixtureDef);

			if (scaledHeight > diameter)
			{
				b2CircleShape bottomShape;
				bottomShape.m_radius = scaledRadius;
				bottomShape.m_p.Set(capsuleColliderComp.offset.x, capsuleColliderComp.offset.y - halfHeight + scaledRadius);

				b2FixtureDef bottomCircleFixtureDef;
				bottomCircleFixtureDef.shape = &bottomShape;
				bottomCircleFixtureDef.userData.pointer = (uintptr_t)entity.GetHandle();
				SetPhysicsMaterial(bottomCircleFixtureDef, capsuleColliderComp.physicsMaterial);
				body->CreateFixture(&bottomCircleFixtureDef);

				b2PolygonShape rectShape;
				rectShape.SetAsBox(scaledRadius, halfHeight - scaledRadius,
					b2Vec2(capsuleColliderComp.offset.x, capsuleColliderComp.offset.y), 0.0f);

				b2FixtureDef rectFixtureDef;
				rectFixtureDef.shape = &rectShape;
				rectFixtureDef.userData.pointer = (uintptr_t)entity.GetHandle();
				SetPhysicsMaterial(rectFixtureDef, capsuleColliderComp.physicsMaterial);
				body->CreateFixture(&rectFixtureDef);
			}
		}
		else
		{
			float scaledHeight = abs(capsuleColliderComp.height * transformComp.scale.x);
			float scaledRadius = abs(capsuleColliderComp.radius * transformComp.scale.y);
			float halfHeight = scaledHeight / 2.0f;
			float diameter = (2.0f * scaledRadius);
			if (scaledHeight < diameter)
				halfHeight = scaledRadius;
			b2CircleShape topShape;
			topShape.m_radius = scaledRadius;
			topShape.m_p.Set(capsuleColliderComp.offset.x + halfHeight - scaledRadius, capsuleColliderComp.offset.y);

			b2FixtureDef topCirclefixtureDef;
			topCirclefixtureDef.shape = &topShape;
			topCirclefixtureDef.userData.pointer = (uintptr_t)entity.GetHandle();

			SetPhysicsMaterial(topCirclefixtureDef, capsuleColliderComp.physicsMaterial);
			body->CreateFixture(&topCirclefixtureDef);

			if (scaledHeight - diameter > 0)
			{
				b2CircleShape bottomShape;
				bottomShape.m_radius = scaledRadius;
				bottomShape.m_p.Set(capsuleColliderComp.offset.x - halfHeight + scaledRadius, capsuleColliderComp.offset.y);

				b2FixtureDef bottomCircleFixtureDef;
				bottomCircleFixtureDef.shape = &bottomShape;
				bottomCircleFixtureDef.userData.pointer = (uintptr_t)entity.GetHandle();
				SetPhysicsMaterial(bottomCircleFixtureDef, capsuleColliderComp.physicsMaterial);
				body->CreateFixture(&bottomCircleFixtureDef);

				b2PolygonShape rectShape;
				rectShape.SetAsBox(halfHeight - scaledRadius, scaledRadius,
					b2Vec2(capsuleColliderComp.offset.x, capsuleColliderComp.offset.y), 0.0f);

				b2FixtureDef rectFixtureDef;
				rectFixtureDef.shape = &rectShape;
				rectFixtureDef.userData.pointer = (uintptr_t)entity.GetHandle();
				SetPhysicsMaterial(rectFixtureDef, capsuleColliderComp.physicsMaterial);
				body->CreateFixture(&rectFixtureDef);
			}
		}
	}

	if (TilemapComponent* tilemapComp = entity.TryGetComponent<TilemapComponent>())
	{
		for (uint32_t row = 0; tilemapComp->tiles.size(); ++row)
		{
			for (uint32_t column = 0 ; tilemapComp->tiles[row].size(); ++column)
			{
				const Tile& tile = tilemapComp->tileset->GetTile(column, row);
				if (tile.GetCollisionShape() != Tile::CollisionShape::None)
				{

				}
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

void RigidBody2DComponent::SetAngularVelocity(float velocity)
{
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