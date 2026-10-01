#pragma once

#include "cereal/cereal.hpp"

#include "math/Vector2f.h"
#include "Asset/PhysicsMaterial.h"
#include "Utilities/SerializationUtils.h"
#include "Scripting/Lua/LuaBindings.h"

class b2Body;

struct CircleCollider2DComponent
{
  Vector2f offset = { 0.0f,0.0f };

  float radius = 0.5f;

  bool isTrigger = false;

  Ref<PhysicsMaterial> physicsMaterial;

  b2Body* runtimeBody = nullptr;

  CircleCollider2DComponent() = default;
  CircleCollider2DComponent(const CircleCollider2DComponent&) = default;
  REFLECT_LUA_BEGIN(CircleCollider2DComponent)
    REFLECT_LUA_PROPERTY_CUSTOM("Offset", "Offset of the shape from the entity's position", "Vector2f",
      ([](Self& c) -> Vector2f& { return c.offset; }),
      ([](Self& c, const Vector2f& v) { c.offset = v; }))
    REFLECT_LUA_PROPERTY_CUSTOM("Radius", "Radius of the circle", "number",
      ([](Self& c) { return c.radius; }),
      ([](Self& c, float v) { c.radius = v; }))
    REFLECT_LUA_PROPERTY_CUSTOM("PhysicsMaterial", "Density, friction and restitution of the shape, or nil for the defaults", "PhysicsMaterial",
      ([](Self& c) { return c.physicsMaterial; }),
      ([](Self& c, const Ref<PhysicsMaterial>& v) { c.physicsMaterial = v; }))
  REFLECT_LUA_END()

private:
  friend cereal::access;
  template<typename Archive>
  void save(Archive& archive) const
  {
    archive(offset, radius, isTrigger);

    SerializationUtils::SaveAssetToArchive(archive, physicsMaterial);
  }

  template<typename Archive>
  void load(Archive& archive)
  {
    archive(offset, radius, isTrigger);
    SerializationUtils::LoadAssetFromArchive(archive, physicsMaterial);

    runtimeBody = nullptr;
  }
};