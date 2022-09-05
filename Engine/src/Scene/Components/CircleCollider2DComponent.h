#pragma once

#include "cereal/cereal.hpp"

#include "math/Vector2f.h"
#include "Physics/PhysicsMaterial.h"

struct CircleCollider2DComponent
{
  Vector2f offset = { 0.0f,0.0f };

  float radius = 0.5f;

  Ref<PhysicsMaterial> physicsMaterial;

  void* RuntimeFixture = nullptr;

  CircleCollider2DComponent() = default;
  CircleCollider2DComponent(const CircleCollider2DComponent&) = default;
private:
  friend cereal::access;
  template<typename Archive>
  void save(Archive& archive) const
  {
    archive(offset, radius);

    std::string relativePath;
    if (physicsMaterial)
    {
      relativePath = FileUtils::RelativePath(physicsMaterial->GetFilepath(), Application::GetOpenDocumentDirectory()).string();
    }
    archive(relativePath);
  }

  template<typename Archive>
  void load(Archive& archive)
  {
    archive(offset, radius);
    std::string relativePath;
    archive(relativePath);
    if (!relativePath.empty())
      physicsMaterial = AssetManager::GetPhysicsMaterial(std::filesystem::absolute(Application::GetOpenDocumentDirectory() / relativePath));
    else
      physicsMaterial = nullptr;
  }
};