#include "stdafx.h"
#include "StaticMesh.h"
#include "Utilities/MeshSerializer.h"

#include "Scene/AssetManager.h"

StaticMesh::StaticMesh(const std::filesystem::path& filepath)
{
	Load(filepath);
}

StaticMesh::StaticMesh(const std::filesystem::path& filepath, const std::vector<uint8_t>& data)
{
	Load(filepath, data);
}

bool StaticMesh::Load(const std::filesystem::path& filepath)
{
	PROFILE_FUNCTION();

	std::filesystem::path absolutePath = std::filesystem::absolute(Application::GetOpenDocumentDirectory() / filepath);
	if (!std::filesystem::exists(absolutePath))
	{
		ENGINE_ERROR("StaticMesh file does not exist: {0}", absolutePath.string());
		return false;
	}

	Ref<Mesh> mesh = MeshSerializer::Deserialize(absolutePath);
	if (!mesh)
		return false;

	m_Filepath = filepath;
	m_Filepath.make_preferred();

	m_Mesh = { mesh };
	return true;
}

bool StaticMesh::Load(const std::filesystem::path& filepath, const std::vector<uint8_t>& data)
{
	Ref<Mesh> mesh = MeshSerializer::DeserializeFromMemory(filepath, data);
	if(!mesh)
		return false;

	m_Filepath = filepath;
	m_Filepath.make_preferred();
	m_Mesh = { mesh };
	return true;
}
