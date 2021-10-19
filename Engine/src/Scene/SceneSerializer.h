#pragma once

#include "Scene.h"
#include "Core/core.h"

class SceneSerializer
{
public:
	SceneSerializer(const Ref<Scene>& scene);

	~SceneSerializer()
	{
	}

	bool Serialize(const std::filesystem::path& filepath);

private:
	Ref<Scene> m_Scene;
};
