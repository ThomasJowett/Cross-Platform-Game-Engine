#pragma once

#include "Scene.h"

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
	bool Deserialize(const std::filesystem::path& filepath, const std::vector<uint8_t>& data);

	static void SerializeEntity(tinyxml2::XMLElement* pElement, Entity entity, tinyxml2::XMLElement* pParentNode = nullptr);
	static Entity DeserializeEntity(Scene* scene, tinyxml2::XMLElement* pEntityElement, bool resetUuid = false);

	static std::string SerializeEntity(Entity entity);
	static Entity DeserializeEntity(Scene* scene, const std::string& prefab, bool resetUuid = false);
private:
	Scene* m_Scene;
};
