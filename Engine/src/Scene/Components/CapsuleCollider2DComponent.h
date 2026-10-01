#pragma once

#include "cereal/cereal.hpp"

#include "math/Vector2f.h"

#include "Asset/PhysicsMaterial.h"
#include "Scripting/Lua/LuaBindings.h"

class b2Body;

struct CapsuleCollider2DComponent
{
	enum class Direction
	{
		Vertical,
		Horizontal
	}direction;

	Vector2f offset = { 0.0f , 0.0f };

	float radius = 0.5f;
	float height = 2.0f;

	bool isTrigger = false;

	Ref<PhysicsMaterial> physicsMaterial;

	b2Body* runtimeBody = nullptr;

	CapsuleCollider2DComponent() = default;
	CapsuleCollider2DComponent(const CapsuleCollider2DComponent&) = default;
	REFLECT_LUA_BEGIN(CapsuleCollider2DComponent)
		REFLECT_LUA_PROPERTY_CUSTOM("Offset", "Offset of the shape from the entity's position", "Vec2",
			([](Self& c) -> Vector2f& { return c.offset; }),
			([](Self& c, const Vector2f& v) { c.offset = v; }))
		REFLECT_LUA_PROPERTY_CUSTOM("Radius", "Radius of the capsule's rounded ends", "number",
			([](Self& c) { return c.radius; }),
			([](Self& c, float v) { c.radius = v; }))
		REFLECT_LUA_PROPERTY_CUSTOM("Height", "Total height of the capsule", "number",
			([](Self& c) { return c.height; }),
			([](Self& c, float v) { c.height = v; }))
		REFLECT_LUA_PROPERTY_CUSTOM("PhysicsMaterial", "Density, friction and restitution of the shape, or nil for the defaults", "PhysicsMaterial",
			([](Self& c) { return c.physicsMaterial; }),
			([](Self& c, const Ref<PhysicsMaterial>& v) { c.physicsMaterial = v; }))
	REFLECT_LUA_END()

private:
	friend cereal::access;
	template<typename Archive>
	void save(Archive& archive) const
	{
		archive(offset, radius, height, direction, isTrigger);

		SerializationUtils::SaveAssetToArchive(archive, physicsMaterial);
	}

	template<typename Archive>
	void load(Archive& archive)
	{
		archive(offset, radius, height, direction, isTrigger);
		SerializationUtils::LoadAssetFromArchive(archive, physicsMaterial);

		runtimeBody = nullptr;
	}
};
