#pragma once

#include "cereal/access.hpp"
#include "cereal/types/vector.hpp"

#include "math/Vector2f.h"
#include "Physics/PhysicsMaterial.h"

#include <vector>

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
