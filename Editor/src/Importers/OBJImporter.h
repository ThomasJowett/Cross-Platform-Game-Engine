#pragma once

#include <filesystem>

class Vector3f;
class Vector2f;

class OBJImporter
{
public:
	static void ImportAssets(const std::filesystem::path& filepath, const std::filesystem::path& destination);

private:
	static void InsertTangentsIntoArray();

	Vector3f CalculateTangent(Vector3f pos0, Vector3f pos1, Vector3f pos2, Vector2f tex0, Vector2f tex1, Vector2f tex2);
};

