#pragma once

#include "Scene.h"
#include "Core/core.h"

namespace tinyxml2
{
class XMLElement;
}

class SceneSerializer
{
public:
	explicit SceneSerializer(Scene* scene);

	~SceneSerializer() = default;

	bool Serialize(const std::filesystem::path& filepath) const;
	bool Deserialize(const std::filesystem::path& filepath);

	static void SerializeEntity(tinyxml2::XMLElement* pElement, Entity entity, tinyxml2::XMLElement* pParentNode = nullptr);
	static Entity DeserializeEntity(Scene* scene, tinyxml2::XMLElement* pEntityElement, bool resetUuid = false);
private:
	Scene* m_Scene;
};
