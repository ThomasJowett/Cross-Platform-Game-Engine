#pragma once

#include "cereal/cereal.hpp"

#include "math/Vector2f.h"
#include "Physics/PhysicsMaterial.h"

#include "Utilities/FileUtils.h"
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
		archive(offset, size, isTrigger);
		std::string relativePath;
		archive(relativePath);
		if (!relativePath.empty())
			physicsMaterial = AssetManager::GetAsset<PhysicsMaterial>(std::filesystem::absolute(Application::GetOpenDocumentDirectory() / relativePath));
		else
			physicsMaterial.reset();

		runtimeBody = nullptr;
	}
};