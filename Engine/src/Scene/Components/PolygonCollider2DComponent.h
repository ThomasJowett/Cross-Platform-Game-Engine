#pragma once

#include "cereal/access.hpp"
#include "cereal/types/vector.hpp"

#include "math/Vector2f.h"
#include "Physics/PhysicsMaterial.h"

#include <vector>

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

	Ref<PhysicsMaterial> physicsMaterial;

	void* runtimeFixture = nullptr;

	PolygonCollider2DComponent() = default;
	PolygonCollider2DComponent(const PolygonCollider2DComponent&) = default;
private:
	friend cereal::access;
	template<typename Archive>
	void save(Archive& archive) const
	{
		archive(offset, vertices);

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
		archive(offset, vertices);
		std::string relativePath;
		archive(relativePath);
		if (!relativePath.empty())
			physicsMaterial = AssetManager::GetPhysicsMaterial(std::filesystem::absolute(Application::GetOpenDocumentDirectory() / relativePath));
		else
			physicsMaterial.reset();
	}
};
