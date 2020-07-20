#include "stdafx.h"
#include "Mesh.h"

static BufferLayout staticMeshLayout = {
		{ShaderDataType::Float3, "a_Position"},
		{ShaderDataType::Float3, "a_Normal"},
		{ShaderDataType::Float3, "a_Tangent"},
		{ShaderDataType::Float2, "a_TexCoord"}
};

Mesh::Mesh(std::filesystem::path filepath)
{
	LoadModel(filepath);
}

Mesh::Mesh(Ref<VertexArray> vertexArray, std::string name)
{
	LoadModel(vertexArray, name);
}

void Mesh::LoadModel(Ref<VertexArray> vertexArray, std::string name)
{
	m_Name = name;
	m_VertexArray = vertexArray;
}

void Mesh::LoadModel(std::filesystem::path filepath)
{
	PROFILE_FUNCTION();

	uint32_t numVertices;
	uint32_t numIndices;

	std::ifstream file;
	file.open(filepath, std::ios::in | std::ios::binary);

	if (!file.good())
		return;

	m_Name = filepath.filename().string();

	m_VertexArray = VertexArray::Create();

	//Read in the array sizes
	file.read((char*)&numVertices, sizeof(uint32_t));
	file.read((char*)&numIndices, sizeof(uint32_t));

	numVertices *= 11;

	//Read in the data
	float* vertices = new float[numVertices];
	uint32_t* indices = new uint32_t[numIndices];

	file.read((char*)vertices, sizeof(float) * numVertices);
	file.read((char*)indices, sizeof(uint32_t) * numIndices);

	Ref<VertexBuffer> vertexBuffer;
	vertexBuffer = VertexBuffer::Create(vertices, sizeof(float) * numVertices);
	Ref<IndexBuffer> indexBuffer;
	indexBuffer = IndexBuffer::Create(indices, numIndices);

	vertexBuffer->SetLayout(staticMeshLayout);

	m_VertexArray->AddVertexBuffer(vertexBuffer);
	m_VertexArray->SetIndexBuffer(indexBuffer);

	delete[] indices;
	delete[] vertices;
}