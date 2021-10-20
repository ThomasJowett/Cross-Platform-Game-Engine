#pragma once

#include "Scene.h"
#include "Core/core.h"

#include "TinyXml2/tinyxml2.h"

class SceneSerializer
{
public:
	SceneSerializer(Scene* scene);

	~SceneSerializer()
	{
	}

	bool Serialize(const std::filesystem::path& filepath);
	bool Deserialize(const std::filesystem::path& filepath);

private:
	void SerializeEntity(tinyxml2::XMLElement* pElement, Entity entity);
	Scene* m_Scene;
};
