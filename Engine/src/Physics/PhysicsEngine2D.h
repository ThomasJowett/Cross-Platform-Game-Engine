#pragma once
#include "Core/core.h"
#include "Contact2D.h"

class Entity;
class Scene;
class b2World;
class b2Draw;
struct HitResult2D;


class PhysicsEngine2D
{
public:
	PhysicsEngine2D(const Vector2f& gravity, Scene* scene);
	~PhysicsEngine2D();
	b2World* GetWorld() { return m_Box2DWorld.get(); }

	void OnFixedUpdate();
	void OnRender();

	void RemoveOldContacts();
	ContactListener2D* GetContactListener() { return m_ContactListener.get(); }

	void InitializeEntity(Entity entity);
	void DestroyEntity(Entity entity);

	void SetGravity(Vector2f gravity);

	HitResult2D RayCast(Vector2f begin, Vector2f end);
	std::vector<HitResult2D> MultiRayCast2D(Vector2f begin, Vector2f end);
	void ShowDebugDraw(bool show);


private:
	Scope<b2World> m_Box2DWorld = nullptr;
	Scope<b2Draw> m_Box2DDraw = nullptr;

	Scope<ContactListener2D> m_ContactListener = nullptr;

	Scene* m_Scene;

	const int32_t m_VelocityIterations = 6;
	const int32_t m_PositionIterations = 2;
};
