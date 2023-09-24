#pragma once

#include "cereal/cereal.hpp"

#include "math/Vector2f.h"
#include "Physics/PhysicsMaterial.h"

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