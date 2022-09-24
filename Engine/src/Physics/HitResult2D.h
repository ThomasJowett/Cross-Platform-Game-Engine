#pragma once

#include "Scene/Entity.h"
#include "Scene/SceneManager.h"
#include "box2d/box2d.h"

struct HitResult2D : public b2RayCastCallback
{
	float ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction) override
	{
		entity = Entity((entt::entity)fixture->GetUserData().pointer, SceneManager::CurrentScene());

		hit = true;
		hitPoint = Vector2f(point.x, point.y);
		hitNormal = Vector2f(normal.x, normal.y);

		return fraction;
	}

	bool hit = false;
	Entity entity;
	Vector2f hitPoint;
	Vector2f hitNormal;

	operator bool() const { return hit; }
};

class RayCastMultipleCallback : public b2RayCastCallback
{
public:
	float ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction) override
	{
		HitResult2D result;
		result.ReportFixture(fixture, point, normal, fraction);

		m_Hits[fraction] = result;

		return 1.0f;
	}

	std::vector<HitResult2D> GetHitResults()
	{
		std::vector<HitResult2D> results;
		for (auto&[_, result] : m_Hits)
		{
			results.push_back(result);
		}
		return results;
	}
private:
	std::map<float, HitResult2D> m_Hits;
};
