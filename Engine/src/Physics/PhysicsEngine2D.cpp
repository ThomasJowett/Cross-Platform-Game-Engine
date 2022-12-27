#include "stdafx.h"
#include "PhysicsEngine2D.h"
#include "HitResult2D.h"
#include "Contact2D.h"

#include "Scene/Entity.h"
#include "Scene/Components/Components.h"
#include "Renderer/Renderer2D.h"
#include "box2d/box2d.h"
#include "Utilities/Box2DDebugDraw.h"
#include "Utilities/Triangulation.h"

uint32_t GetRigidBodyBox2DType(RigidBody2DComponent::BodyType type)
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

PhysicsEngine2D::PhysicsEngine2D(const Vector2f& gravity, Scene* scene)
	:m_Scene(scene)
{
	m_Box2DWorld = CreateScope<b2World>(b2Vec2(gravity.x, gravity.y));
	m_ContactListener = CreateScope<ContactListener2D>();
	m_Box2DWorld->SetContactListener(m_ContactListener.get());
}

PhysicsEngine2D::~PhysicsEngine2D()
{
}

void PhysicsEngine2D::OnFixedUpdate()
{
	m_Box2DWorld->Step(Application::Get().GetFixedUpdateInterval(), m_VelocityIterations, m_PositionIterations);

	m_Scene->GetRegistry().view<TransformComponent, RigidBody2DComponent>().each([=](auto entity, auto& transformComp, auto& rigidBodyComp)
		{
			if (rigidBodyComp.runtimeBody == nullptr
				|| rigidBodyComp.runtimeBody->GetType() != GetRigidBodyBox2DType(rigidBodyComp.type))
			{
				Entity e(entity, m_Scene);
				InitializeEntity(e);
			}
			else if (rigidBodyComp.type == RigidBody2DComponent::BodyType::DYNAMIC)
			{
				rigidBodyComp.runtimeBody->SetFixedRotation(rigidBodyComp.fixedRotation);
				rigidBodyComp.runtimeBody->SetAngularDamping(rigidBodyComp.angularDamping);
				rigidBodyComp.runtimeBody->SetLinearDamping(rigidBodyComp.linearDamping);
				rigidBodyComp.runtimeBody->SetGravityScale(rigidBodyComp.gravityScale);
			}
			const b2Vec2& position = rigidBodyComp.runtimeBody->GetPosition();
			transformComp.position.x = position.x;
			transformComp.position.y = position.y;
			transformComp.rotation.z = (float)rigidBodyComp.runtimeBody->GetAngle();
		});
}

void PhysicsEngine2D::OnRender()
{
	if (m_Box2DDraw)
		m_Box2DWorld->DebugDraw();
}

void PhysicsEngine2D::RemoveOldContacts()
{
	m_ContactListener->RemoveOld();
}

void PhysicsEngine2D::InitializeEntity(Entity entity)
{
	if (RigidBody2DComponent* rigidBodyComp = entity.TryGetComponent<RigidBody2DComponent>())
	{
		TransformComponent& transformComp = entity.GetComponent<TransformComponent>();

		b2BodyDef bodyDef;
		bodyDef.type = (b2BodyType)GetRigidBodyBox2DType(rigidBodyComp->type);
		bodyDef.position = b2Vec2(transformComp.position.x, transformComp.position.y);
		bodyDef.angle = (float)transformComp.rotation.z;

		if (rigidBodyComp->type == RigidBody2DComponent::BodyType::DYNAMIC)
		{
			bodyDef.fixedRotation = rigidBodyComp->fixedRotation;
			bodyDef.angularDamping = rigidBodyComp->angularDamping;
			bodyDef.linearDamping = rigidBodyComp->linearDamping;
			bodyDef.gravityScale = rigidBodyComp->gravityScale;
		}

		b2Body* body = m_Box2DWorld->CreateBody(&bodyDef);

		rigidBodyComp->runtimeBody = body;

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
			fixtureDef.isSensor = boxColliderComp.isTrigger;
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
				b2Fixture* fixture = body->CreateFixture(&topCirclefixtureDef);

				if (luaScriptComponent)
					luaScriptComponent->m_Fixtures.push_back(fixture);

				if (scaledHeight > diameter)
				{
					b2CircleShape bottomShape;
					bottomShape.m_radius = scaledRadius;
					bottomShape.m_p.Set(capsuleColliderComp.offset.x, capsuleColliderComp.offset.y - halfHeight + scaledRadius);

					b2FixtureDef bottomCircleFixtureDef;
					bottomCircleFixtureDef.shape = &bottomShape;
					bottomCircleFixtureDef.userData.pointer = (uintptr_t)entity.GetHandle();
					SetPhysicsMaterial(bottomCircleFixtureDef, capsuleColliderComp.physicsMaterial);
					fixture = body->CreateFixture(&bottomCircleFixtureDef);

					if (luaScriptComponent)
						luaScriptComponent->m_Fixtures.push_back(fixture);

					b2PolygonShape rectShape;
					rectShape.SetAsBox(scaledRadius, halfHeight - scaledRadius,
						b2Vec2(capsuleColliderComp.offset.x, capsuleColliderComp.offset.y), 0.0f);

					b2FixtureDef rectFixtureDef;
					rectFixtureDef.shape = &rectShape;
					rectFixtureDef.userData.pointer = (uintptr_t)entity.GetHandle();
					SetPhysicsMaterial(rectFixtureDef, capsuleColliderComp.physicsMaterial);
					fixture = body->CreateFixture(&rectFixtureDef);

					if (luaScriptComponent)
						luaScriptComponent->m_Fixtures.push_back(fixture);
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
				b2Fixture* fixture = body->CreateFixture(&topCirclefixtureDef);

				if (luaScriptComponent)
					luaScriptComponent->m_Fixtures.push_back(fixture);

				if (scaledHeight - diameter > 0)
				{
					b2CircleShape bottomShape;
					bottomShape.m_radius = scaledRadius;
					bottomShape.m_p.Set(capsuleColliderComp.offset.x - halfHeight + scaledRadius, capsuleColliderComp.offset.y);

					b2FixtureDef bottomCircleFixtureDef;
					bottomCircleFixtureDef.shape = &bottomShape;
					bottomCircleFixtureDef.userData.pointer = (uintptr_t)entity.GetHandle();
					SetPhysicsMaterial(bottomCircleFixtureDef, capsuleColliderComp.physicsMaterial);
					fixture = body->CreateFixture(&bottomCircleFixtureDef);

					if (luaScriptComponent)
						luaScriptComponent->m_Fixtures.push_back(fixture);

					b2PolygonShape rectShape;
					rectShape.SetAsBox(halfHeight - scaledRadius, scaledRadius,
						b2Vec2(capsuleColliderComp.offset.x, capsuleColliderComp.offset.y), 0.0f);

					b2FixtureDef rectFixtureDef;
					rectFixtureDef.shape = &rectShape;
					rectFixtureDef.userData.pointer = (uintptr_t)entity.GetHandle();
					SetPhysicsMaterial(rectFixtureDef, capsuleColliderComp.physicsMaterial);
					fixture = body->CreateFixture(&rectFixtureDef);

					if (luaScriptComponent)
						luaScriptComponent->m_Fixtures.push_back(fixture);
				}
			}
		}

		if (const TilemapComponent* tilemapComp = entity.TryGetComponent<TilemapComponent>())
		{
			float tileWidth = transformComp.scale.x;
			float tileHieght = transformComp.scale.y;

			uint32_t i = 0;
			for (const auto& row : tilemapComp->tiles)
			{
				uint32_t j = 0;
				for (uint32_t index : row)
				{
					if (index > 0)
					{
						const Tile& tile = tilemapComp->tileset->GetTile(index - 1);
						if (tile.GetCollisionShape() != Tile::CollisionShape::None)
						{
							// TODO: add a fixture for tilemaps
							if (tile.GetCollisionShape() == Tile::CollisionShape::Rect)
							{
								b2Vec2 center = b2Vec2(j * tileWidth + (0.5f * tileWidth), -(i * tileHieght + (0.5f * tileHieght)));
								b2PolygonShape rectShape;
								rectShape.SetAsBox(tileWidth * 0.5f, tileHieght * 0.5f, center, 0.0f);

								b2FixtureDef rectFixtureDef;
								rectFixtureDef.shape = &rectShape;
								Ref<PhysicsMaterial> defaultPhysicsMaterial = PhysicsMaterial::GetDefaultPhysicsMaterial();
								rectFixtureDef.density = defaultPhysicsMaterial->GetDensity();
								rectFixtureDef.friction = defaultPhysicsMaterial->GetFriction();
								rectFixtureDef.restitution = defaultPhysicsMaterial->GetRestitution();
								rectFixtureDef.userData.pointer = (uintptr_t)entity.GetHandle();
								b2Fixture* fixture = body->CreateFixture(&rectFixtureDef);

								if (luaScriptComponent)
									luaScriptComponent->m_Fixtures.push_back(fixture);
							}
						}
					}
					j++;
				}
				i++;
			}
		}
	}
}

void PhysicsEngine2D::DestroyEntity(Entity entity)
{
	if (RigidBody2DComponent* rigidBodyComp = entity.TryGetComponent<RigidBody2DComponent>())
		m_Box2DWorld->DestroyBody(rigidBodyComp->runtimeBody);
	//else if(BoxCollider2DComponent* boxColliderComp = entity.TryGetComponent<BoxCollider2DComponent>())
	//	m_Box2DWorld->DestroyBody(boxColliderComp->)
}

void PhysicsEngine2D::SetGravity(Vector2f gravity)
{
	m_Box2DWorld->SetGravity(b2Vec2(gravity.x, gravity.y));
}

HitResult2D PhysicsEngine2D::RayCast(Vector2f begin, Vector2f end)
{
	HitResult2D callback;
	m_Box2DWorld->RayCast(&callback, b2Vec2(begin.x, begin.y), b2Vec2(end.x, end.y));

	if (callback.hit)
	{
		Renderer2D::DrawHairLine(Vector3f(begin.x, begin.y, 0.0f), Vector3f(callback.hitPoint.x, callback.hitPoint.y, 0.0f), Colours::LIME_GREEN);
	}
	else
	{
		Renderer2D::DrawHairLine(Vector3f(begin.x, begin.y, 0.0f), Vector3f(end.x, end.y, 0.0f), Colours::RED);
	}
	return callback;
}

std::vector<HitResult2D> PhysicsEngine2D::MultiRayCast2D(Vector2f begin, Vector2f end)
{
	RayCastMultipleCallback callback;
	m_Box2DWorld->RayCast(&callback, b2Vec2(begin.x, begin.y), b2Vec2(end.x, end.y));

	return callback.GetHitResults();
}

void PhysicsEngine2D::ShowDebugDraw(bool show)
{
	if (show) {
		m_Box2DDraw = CreateScope<Box2DDebugDraw>();
		m_Box2DDraw->SetFlags(b2Draw::e_shapeBit);
		m_Box2DWorld->SetDebugDraw(m_Box2DDraw.get());
	}
	else
	{
		m_Box2DWorld->SetDebugDraw(nullptr);
		m_Box2DDraw.reset();
	}
}
