#pragma once

#include "Renderer/Mesh.h"

class MeshSerializer
{
public:
	static bool Serialize(const std::filesystem::path& filepath, const Ref<Mesh>& mesh);
	static Ref<Mesh> DeserializeFromMemory(const std::filesystem::path& filepath, const std::vector<uint8_t>& data);
	static Ref<Mesh> Deserialize(const std::filesystem::path& filepath);
};
