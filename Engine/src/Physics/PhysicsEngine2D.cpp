#include "stdafx.h"
#include "PhysicsEngine2D.h"
#include "HitResult2D.h"
#include "Contact2D.h"

#include "Scene/Entity.h"
#include "Scene/SceneGraph.h"
#include "Scene/Components/RigidBody2DComponent.h"
#include "Scene/Components/BoxCollider2DComponent.h"
#include "Scene/Components/CircleCollider2DComponent.h"
#include "Scene/Components/PolygonCollider2DComponent.h"
#include "Scene/Components/CapsuleCollider2DComponent.h"
#include "Scene/Components/TilemapComponent.h"
#include "Scene/Components/LuaScriptComponent.h"
#include "Scene/Components/HierarchyComponent.h"
#include "Scene/Components/WeldJoint2DComponent.h"
#include "Renderer/Renderer2D.h"
#include "box2d/box2d.h"
#include "Utilities/Box2DDebugDraw.h"
#include "Utilities/Triangulation.h"
#include "Logging/Instrumentor.h"

#define COLLIDER_COMPONENTS	\
BoxCollider2DComponent,		\
CircleCollider2DComponent,	\
PolygonCollider2DComponent,	\
CapsuleCollider2DComponent,	\
TilemapComponent			\

#define JOINT_COMPONENTS \
WeldJoint2DComponent	 \

uint32_t GetRigidBodyBox2DType(RigidBody2DComponent::BodyType type)
{
	PROFILE_FUNCTION();
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
	PROFILE_FUNCTION();
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
	PROFILE_FUNCTION();
	m_Box2DWorld = CreateScope<b2World>(b2Vec2(gravity.x, gravity.y));
	m_ContactListener = CreateScope<ContactListener2D>();
	m_Box2DWorld->SetContactListener(m_ContactListener.get());

	b2BodyDef worldBodyDef;
	worldBodyDef.position.Set(0.0f, 0.0f);
	worldBodyDef.type = b2_staticBody;

	m_WorldBody = m_Box2DWorld->CreateBody(&worldBodyDef);

	m_Scene->GetRegistry().view<TransformComponent>(entt::exclude<HierarchyComponent>).each(
		[this](const auto physicsEntity, auto& transformComp)
		{
			InitializeEntity({ physicsEntity, m_Scene });
		});

	auto& registry = m_Scene->GetRegistry();

	auto view = registry.view<TransformComponent, HierarchyComponent>();

	for (auto entityHandle : view)
	{
		auto& hierarchyComp = view.get<HierarchyComponent>(entityHandle);
		if (hierarchyComp.parent == entt::null && hierarchyComp.isActive)
		{
			InitializeEntity({ entityHandle, m_Scene });
		}
	}
}

PhysicsEngine2D::~PhysicsEngine2D()
{
}

void PhysicsEngine2D::OnFixedUpdate()
{
	PROFILE_FUNCTION();
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
	PROFILE_FUNCTION();
	if (m_Box2DDraw)
		m_Box2DWorld->DebugDraw();
}

void PhysicsEngine2D::RemoveOldContacts()
{
	PROFILE_FUNCTION();
	m_ContactListener->RemoveOld();
}

void PhysicsEngine2D::InitializeEntity(Entity entity)
{
	PROFILE_FUNCTION();
	b2Body* body = nullptr;
	TransformComponent& transformComp = entity.GetComponent<TransformComponent>();

	HierarchyComponent* hierarchyComp = entity.TryGetComponent<HierarchyComponent>();

	entt::entity parent = entt::null;
	entt::entity firstChild = entt::null;
	entt::entity nextSibling = entt::null;

	if (hierarchyComp && hierarchyComp->parent != entt::null)
	{
		parent = hierarchyComp->parent;
		firstChild = hierarchyComp->firstChild;
		nextSibling = hierarchyComp->nextSibling;
		Vector3f position;
		Vector3f rotation;
		Vector3f scale;
		Matrix4x4 transform = transformComp.GetWorldMatrix();

		transformComp.GetWorldMatrix().Decompose(position, rotation, scale);
		SceneGraph::Unparent(entity);
		transformComp.position = position;
		transformComp.rotation = rotation;
		transformComp.scale = scale;
	}

	if (RigidBody2DComponent* rigidBodyComp = entity.TryGetComponent<RigidBody2DComponent>())
	{
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

		body = m_Box2DWorld->CreateBody(&bodyDef);

		rigidBodyComp->runtimeBody = body;
	}
	else if (entity.HasComponent<COLLIDER_COMPONENTS>())
	{
		b2BodyDef bodyDef;
		bodyDef.type = b2BodyType::b2_kinematicBody;
		bodyDef.position = b2Vec2(transformComp.position.x, transformComp.position.y);
		bodyDef.angle = (float)transformComp.rotation.z;
		body = m_Box2DWorld->CreateBody(&bodyDef);
	}
	else
	{
		return;
	}

	LuaScriptComponent* luaScriptComponent = entity.TryGetComponent<LuaScriptComponent>();

	if (luaScriptComponent && !luaScriptComponent->IsContactListener())
		luaScriptComponent = nullptr;

	if (entity.HasComponent<BoxCollider2DComponent>())
	{
		auto& boxColliderComp = entity.GetComponent<BoxCollider2DComponent>();

		constexpr float MinBoxSize = 0.001f;

		if(std::abs(boxColliderComp.size.x) <= MinBoxSize)
			boxColliderComp.size.x = MinBoxSize;
		if (std::abs(boxColliderComp.size.y) <= MinBoxSize)
			boxColliderComp.size.y = MinBoxSize;

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

		boxColliderComp.runtimeBody = body;
	}

	if (entity.HasComponent<CircleCollider2DComponent>())
	{
		auto& circleColliderComp = entity.GetComponent<CircleCollider2DComponent>();

		b2CircleShape circleShape;
		circleShape.m_radius = abs(circleColliderComp.radius * std::max(transformComp.scale.x, transformComp.scale.y));
		circleShape.m_p.Set(circleColliderComp.offset.x, circleColliderComp.offset.y);

		b2FixtureDef fixtureDef;
		fixtureDef.shape = &circleShape;
		fixtureDef.isSensor = circleColliderComp.isTrigger;
		SetPhysicsMaterial(fixtureDef, circleColliderComp.physicsMaterial);
		fixtureDef.userData.pointer = (uintptr_t)entity.GetHandle();

		b2Fixture* fixture = body->CreateFixture(&fixtureDef);
		if (luaScriptComponent)
			luaScriptComponent->m_Fixtures.push_back(fixture);
		circleColliderComp.runtimeBody = body;
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
				fixtureDef.isSensor = polygonColliderComp.isTrigger;
				SetPhysicsMaterial(fixtureDef, polygonColliderComp.physicsMaterial);

				fixtureDef.userData.pointer = (uintptr_t)entity.GetHandle();

				b2Fixture* fixture = body->CreateFixture(&fixtureDef);
				if (luaScriptComponent)
					luaScriptComponent->m_Fixtures.push_back(fixture);

				polygonColliderComp.runtimeBody = body;
			}
		}
	}

	if (entity.HasComponent<CapsuleCollider2DComponent>())
	{
		auto& capsuleColliderComp = entity.GetComponent<CapsuleCollider2DComponent>();
		capsuleColliderComp.runtimeBody = body;
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
			topCirclefixtureDef.isSensor = capsuleColliderComp.isTrigger;

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
				bottomCircleFixtureDef.isSensor = capsuleColliderComp.isTrigger;
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
				rectFixtureDef.isSensor = capsuleColliderComp.isTrigger;
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
			topCirclefixtureDef.isSensor = capsuleColliderComp.isTrigger;
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
				bottomCircleFixtureDef.isSensor = capsuleColliderComp.isTrigger;
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
				rectFixtureDef.isSensor = capsuleColliderComp.isTrigger;
				rectFixtureDef.userData.pointer = (uintptr_t)entity.GetHandle();
				SetPhysicsMaterial(rectFixtureDef, capsuleColliderComp.physicsMaterial);
				fixture = body->CreateFixture(&rectFixtureDef);

				if (luaScriptComponent)
					luaScriptComponent->m_Fixtures.push_back(fixture);
			}
		}
	}

	if (TilemapComponent* tilemapComp = entity.TryGetComponent<TilemapComponent>();
		tilemapComp && tilemapComp->tileset && tilemapComp->tileset->HasCollision())
	{
		float tileWidth = transformComp.scale.x;
		float tileHeight = transformComp.scale.y;

		uint32_t i = 0;
		for (const auto& row : tilemapComp->tiles)
		{
			uint32_t j = 0;
			for (uint32_t index : row)
			{
				if (index > 0)
				{
					if (tilemapComp->orientation == TilemapComponent::Orientation::orthogonal)
					{
						const Tile& tile = tilemapComp->tileset->GetTile(index - 1);
						if (tile.GetCollisionShape() != Tile::CollisionShape::None)
						{
							// TODO: add a fixture for tilemaps
							if (tile.GetCollisionShape() == Tile::CollisionShape::Rect)
							{
								b2Vec2 center = b2Vec2(j * tileWidth + (0.5f * tileWidth), -(i * tileHeight + (0.5f * tileHeight)));
								b2PolygonShape rectShape;
								rectShape.SetAsBox(tileWidth * 0.5f, tileHeight * 0.5f, center, 0.0f);

								b2FixtureDef rectFixtureDef;
								rectFixtureDef.shape = &rectShape;
								rectFixtureDef.isSensor = tilemapComp->isTrigger;
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
				}
				j++;
			}
			i++;
		}

		tilemapComp->runtimeBody = body;
	}

	if (WeldJoint2DComponent* weldJointComp = entity.TryGetComponent<WeldJoint2DComponent>())
	{
		b2WeldJointDef jointDef;
		weldJointComp->bodyA = body;
		weldJointComp->entityA = entity.GetHandle();
		weldJointComp->entityB = parent;

		if (weldJointComp->entityB != entt::null) {
			if (RigidBody2DComponent* rigidBodyComp = m_Scene->GetRegistry().try_get<RigidBody2DComponent>(weldJointComp->entityB)) {
				weldJointComp->bodyB = rigidBodyComp->runtimeBody;
			}
			else if (TilemapComponent* tilemapComp = m_Scene->GetRegistry().try_get<TilemapComponent>(weldJointComp->entityB)) {
				weldJointComp->bodyB = tilemapComp->runtimeBody;
			}
			else {
				weldJointComp->bodyB = m_WorldBody;
			}
		} else {
			weldJointComp->bodyB = m_WorldBody;
		}
		b2Vec2 worldAnchor = weldJointComp->bodyB->GetWorldCenter();

		jointDef.bodyA = weldJointComp->bodyA;
		jointDef.bodyB = weldJointComp->bodyB;
		jointDef.localAnchorA = weldJointComp->bodyA->GetLocalPoint(worldAnchor);
		jointDef.localAnchorB = weldJointComp->bodyB->GetLocalPoint(worldAnchor);
		jointDef.referenceAngle = jointDef.bodyB->GetAngle() - jointDef.bodyA->GetAngle();
		jointDef.collideConnected = weldJointComp->collideConnected;
		jointDef.damping = weldJointComp->damping;
		jointDef.stiffness = weldJointComp->stiffness;
		jointDef.userData.pointer = (uintptr_t)entity.GetHandle();
		weldJointComp->joint = (b2WeldJoint*)m_Box2DWorld->CreateJoint(&jointDef);
	}

	// Initialize the first child
	if (firstChild != entt::null)
	{
		InitializeEntity({ firstChild, m_Scene });
	}

	// Initialize the next sibling
	if (nextSibling != entt::null)
	{
		InitializeEntity({ nextSibling, m_Scene });
	}
}

void PhysicsEngine2D::DestroyEntity(Entity entity)
{
	PROFILE_FUNCTION();
	if (RigidBody2DComponent* rigidBodyComp = entity.TryGetComponent<RigidBody2DComponent>())
		m_Box2DWorld->DestroyBody(rigidBodyComp->runtimeBody);
	else if (BoxCollider2DComponent* boxColliderComp = entity.TryGetComponent<BoxCollider2DComponent>())
		m_Box2DWorld->DestroyBody((b2Body *)boxColliderComp->runtimeBody);
	else if (CircleCollider2DComponent* colliderComp = entity.TryGetComponent<CircleCollider2DComponent>())
		m_Box2DWorld->DestroyBody((b2Body*)colliderComp->runtimeBody);
	else if (PolygonCollider2DComponent* colliderComp = entity.TryGetComponent<PolygonCollider2DComponent>())
		m_Box2DWorld->DestroyBody((b2Body*)colliderComp->runtimeBody);
	else if (CapsuleCollider2DComponent* colliderComp = entity.TryGetComponent<CapsuleCollider2DComponent>())
		m_Box2DWorld->DestroyBody((b2Body*)colliderComp->runtimeBody);
	else if (TilemapComponent* colliderComp = entity.TryGetComponent<TilemapComponent>())
		m_Box2DWorld->DestroyBody((b2Body*)colliderComp->runtimeBody);

	if (WeldJoint2DComponent* weldJointComp = entity.TryGetComponent<WeldJoint2DComponent>())
		m_Box2DWorld->DestroyJoint(weldJointComp->joint);
}

void PhysicsEngine2D::SetGravity(Vector2f gravity)
{
	PROFILE_FUNCTION();
	m_Box2DWorld->SetGravity(b2Vec2(gravity.x, gravity.y));
}

HitResult2D PhysicsEngine2D::RayCast(Vector2f begin, Vector2f end)
{
	PROFILE_FUNCTION();
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
	PROFILE_FUNCTION();
	RayCastMultipleCallback callback;
	m_Box2DWorld->RayCast(&callback, b2Vec2(begin.x, begin.y), b2Vec2(end.x, end.y));

	return callback.GetHitResults();
}

void PhysicsEngine2D::ShowDebugDraw(bool show)
{
	PROFILE_FUNCTION();
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
