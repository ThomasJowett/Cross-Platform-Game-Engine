#include "stdafx.h"
#include "StaticMesh.h"

#include "Scene/AssetManager.h"

StaticMesh::StaticMesh(const std::filesystem::path& filepath)
{
	Load(filepath);
}

bool StaticMesh::Load(const std::filesystem::path& filepath)
{
	PROFILE_FUNCTION();

	std::ifstream file;
	file.open(filepath, std::ios::in | std::ios::binary);

	if (!file.good())
	{
		ENGINE_ERROR("Failed to load staticmesh from {0}", filepath.string());
		return false;
	}

	m_Filepath = filepath;

	std::filesystem::path assetDirectory = filepath;
	assetDirectory.remove_filename();

	size_t meshCount;

	file.read((char*)&meshCount, sizeof(size_t));

	for (size_t i = 0; i < meshCount; i++)
	{
		size_t numVertices;
		size_t numIndices;
	
		Ref<VertexArray> vertexArray = VertexArray::Create();
		
		//Read in the material name
		std::string materialPath;
		size_t materialPathSize;
	
		file >> materialPathSize;
		
		std::vector<char> tmp(materialPathSize);
		file.read(tmp.data(), materialPathSize); // deserialize characters of string
		materialPath.assign(tmp.data(), materialPathSize);
		if(!materialPath.empty())
			materialPath += ".material";
	
		//Read in the array sizes
		file.read((char*)&numVertices, sizeof(size_t));
		file.read((char*)&numIndices, sizeof(size_t));

		if (numVertices > UINT_MAX || numIndices > UINT_MAX)
		{
			ENGINE_ERROR("Static mesh could not be read");
			return false;
		}
		Vertex* vertices = new Vertex[numVertices];
		uint32_t* indices = new uint32_t[numIndices];
		
		//Read in the arrays
		file.read((char*)vertices, sizeof(Vertex) * numVertices);
		file.read((char*)indices, sizeof(uint32_t) * numIndices);

		std::vector<Vertex> verticesArr;
		verticesArr.assign(numVertices, vertices[0]);
	
		Ref<VertexBuffer> vertexBuffer = VertexBuffer::Create(sizeof(Vertex) * (uint32_t)numVertices);
		vertexBuffer->SetData(vertices);
		Ref<IndexBuffer> indexBuffer = IndexBuffer::Create(indices, (uint32_t)numIndices);
	
		vertexBuffer->SetLayout(s_StaticMeshLayout);
	
		vertexArray->AddVertexBuffer(vertexBuffer);
		vertexArray->SetIndexBuffer(indexBuffer);
	
		BoundingBox aabb;
		//TODO: load aabb from file
	
		Ref<Mesh> mesh = CreateRef<Mesh>(vertexArray);
		if(!materialPath.empty())
			mesh->SetMaterial(AssetManager::GetMaterial(std::filesystem::absolute(assetDirectory / materialPath)));
	
		m_Meshes.push_back(mesh);
	
		delete[] indices;
		delete[] vertices;
	}

	file.close();

	return true;
}

const std::filesystem::path& StaticMesh::GetFilepath() const
{
	return m_Filepath;
}
