#pragma once

#include "cereal/cereal.hpp"

#include "math/Vector2f.h"

#include "Asset/PhysicsMaterial.h"

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
