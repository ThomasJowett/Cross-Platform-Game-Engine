#pragma once

#include "cereal/access.hpp"
#include "cereal/types/vector.hpp"

#include "math/Vector2f.h"
#include "Asset/PhysicsMaterial.h"
#include "Utilities/SerializationUtils.h"

#include <vector>
#include "Scripting/Lua/LuaBindings.h"

class b2Body;

struct PolygonCollider2DComponent
{
	std::vector<Vector2f> vertices = 
	{
		{0.0f, 1.0f},
		{-0.9510565f, 0.309017f},
		{-0.5877852f, -0.8090171f},
		{0.5877854f,-0.8090169f},
		{0.9510565f, 0.3090171f}
	};

	Vector2f offset;
	bool isTrigger;

	Ref<PhysicsMaterial> physicsMaterial;

	b2Body* runtimeBody = nullptr;

	PolygonCollider2DComponent() = default;
	PolygonCollider2DComponent(const PolygonCollider2DComponent&) = default;
	REFLECT_LUA_BEGIN(PolygonCollider2DComponent)
		REFLECT_LUA_PROPERTY_CUSTOM("Offset", "Offset of the shape from the entity's position", "Vector2f",
			([](Self& c) -> Vector2f& { return c.offset; }),
			([](Self& c, const Vector2f& v) { c.offset = v; }))
		REFLECT_LUA_PROPERTY_CUSTOM("Vertices", "The polygon's corners, relative to the entity's position", "table of Vector2f",
			([](Self& c) { return c.vertices; }),
			([](Self& c, const std::vector<Vector2f>& v) { c.vertices = v; }))
		REFLECT_LUA_PROPERTY_CUSTOM("PhysicsMaterial", "Density, friction and restitution of the shape, or nil for the defaults", "PhysicsMaterial",
			([](Self& c) { return c.physicsMaterial; }),
			([](Self& c, const Ref<PhysicsMaterial>& v) { c.physicsMaterial = v; }))
	REFLECT_LUA_END()

private:
	friend cereal::access;
	template<typename Archive>
	void save(Archive& archive) const
	{
		archive(offset, vertices, isTrigger);

		SerializationUtils::SaveAssetToArchive(archive, physicsMaterial);
	}

	template<typename Archive>
	void load(Archive& archive)
	{
		archive(offset, vertices, isTrigger);
		SerializationUtils::LoadAssetFromArchive(archive, physicsMaterial);

		runtimeBody = nullptr;
	}
};
