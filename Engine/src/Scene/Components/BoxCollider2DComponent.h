#pragma once

#include "cereal/cereal.hpp"

#include "math/Vector2f.h"
#include "Asset/PhysicsMaterial.h"

#include "Utilities/FileUtils.h"
#include "Utilities/SerializationUtils.h"
#include "Core/Application.h"
#include "Scene/AssetManager.h"

struct BoxCollider2DComponent
{
	Vector2f offset = { 0.0f, 0.0f };
	Vector2f size = { 0.5f, 0.5f };

	bool isTrigger = false;

	Ref<PhysicsMaterial> physicsMaterial;

	b2Body* runtimeBody = nullptr;

	BoxCollider2DComponent() = default;
	BoxCollider2DComponent(const BoxCollider2DComponent&) = default;
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