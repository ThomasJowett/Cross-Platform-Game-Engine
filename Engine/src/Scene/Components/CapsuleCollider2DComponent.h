#pragma once

#include "cereal/cereal.hpp"

#include "math/Vector2f.h"

#include "Physics/PhysicsMaterial.h"

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

	Ref<PhysicsMaterial> physicsMaterial;

	void* runtimeFixture = nullptr;

	CapsuleCollider2DComponent() = default;
	CapsuleCollider2DComponent(const CapsuleCollider2DComponent&) = default;
private:
	friend cereal::access;
	template<typename Archive>
	void save(Archive& archive) const
	{
		archive(offset, radius, height, direction);

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
		archive(offset, radius, height, direction);
		std::string relativePath;
		archive(relativePath);
		if (!relativePath.empty())
			physicsMaterial = AssetManager::GetPhysicsMaterial(std::filesystem::absolute(Application::GetOpenDocumentDirectory() / relativePath));
		else
			physicsMaterial = nullptr;
	}
};
