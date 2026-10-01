#pragma once

#include "cereal/cereal.hpp"

#include "math/Vector2f.h"
#include "Asset/PhysicsMaterial.h"

#include "Utilities/FileUtils.h"
#include "Utilities/SerializationUtils.h"
#include "Core/Application.h"
#include "Scene/AssetManager.h"
#include "Scripting/Lua/LuaBindings.h"

struct BoxCollider2DComponent
{
	Vector2f offset = { 0.0f, 0.0f };
	Vector2f size = { 0.5f, 0.5f };

	bool isTrigger = false;

	Ref<PhysicsMaterial> physicsMaterial;

	b2Body* runtimeBody = nullptr;

	BoxCollider2DComponent() = default;
	BoxCollider2DComponent(const BoxCollider2DComponent&) = default;
	REFLECT_LUA_BEGIN(BoxCollider2DComponent)
		REFLECT_LUA_PROPERTY_CUSTOM("Offset", "Offset of the shape from the entity's position", "Vector2f",
			([](Self& c) -> Vector2f& { return c.offset; }),
			([](Self& c, const Vector2f& v) { c.offset = v; }))
		REFLECT_LUA_PROPERTY_CUSTOM("Size", "Half the width and height of the box", "Vector2f",
			([](Self& c) -> Vector2f& { return c.size; }),
			([](Self& c, const Vector2f& v) { c.size = v; }))
		REFLECT_LUA_PROPERTY_CUSTOM("PhysicsMaterial", "Density, friction and restitution of the shape, or nil for the defaults", "PhysicsMaterial",
			([](Self& c) { return c.physicsMaterial; }),
			([](Self& c, const Ref<PhysicsMaterial>& v) { c.physicsMaterial = v; }))
	REFLECT_LUA_END()

private:
	friend cereal::access;
	template<typename Archive>
	void save(Archive& archive) const
	{
		archive(offset, size, isTrigger);

		SerializationUtils::SaveAssetToArchive(archive, physicsMaterial);
	}

	template<typename Archive>
	void load(Archive& archive)
	{
		archive(offset, size, isTrigger);
		SerializationUtils::LoadAssetFromArchive(archive, physicsMaterial);

		runtimeBody = nullptr;
	}
};