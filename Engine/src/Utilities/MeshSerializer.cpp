#include "MeshSerializer.h"

#include <fstream>

#define MESH_MAGIC 0x4D455348 // "MESH"
#define MESH_VERSION 1

static void SerializeBufferLayout(std::ostream& out, const BufferLayout& layout)
{
	uint32_t elementCount = static_cast<uint32_t>(layout.GetElements().size());
	out.write((char*)&elementCount, sizeof(uint32_t));

	for (const auto& element : layout.GetElements())
	{
		uint32_t nameLength = static_cast<uint32_t>(element.name.size());
		out.write((char*)&nameLength, sizeof(uint32_t));
		out.write(element.name.data(), nameLength);

		uint32_t type = static_cast<uint32_t>(element.type);
		out.write((char*)&type, sizeof(uint32_t));

		out.write((char*)&element.normalized, sizeof(bool));
	}
}

bool MeshSerializer::Serialize(const std::filesystem::path& filepath, const Ref<Mesh>& mesh)
{
    std::ofstream out(filepath, std::ios::binary);
    if (!out.is_open())
    {
        CLIENT_ERROR("Could not open file {0} for writing", filepath.string());
        return false;
	}

    // Header
	uint32_t magic = MESH_MAGIC;
	uint32_t version = MESH_VERSION;
	out.write(reinterpret_cast<const char*>(&magic), sizeof(magic));
	out.write(reinterpret_cast<const char*>(&version), sizeof(version));

	// Vertex/Index buffer
    const auto& vertices = mesh->GetVertexList();
    const auto& indices = mesh->GetIndexList();
    uint32_t vertexCount = static_cast<uint32_t>(vertices.size());
	uint32_t indexCount = static_cast<uint32_t>(indices.size());

	out.write(reinterpret_cast<const char*>(&vertexCount), sizeof(vertexCount));
	out.write(reinterpret_cast<const char*>(vertices.data()), vertexCount * sizeof(float));

	out.write(reinterpret_cast<const char*>(&indexCount), sizeof(indexCount));
	out.write(reinterpret_cast<const char*>(indices.data()), indexCount * sizeof(uint32_t));

	// Submeshes
	const auto& submeshes = mesh->GetSubmeshes();
	uint32_t submeshCount = static_cast<uint32_t>(submeshes.size());
	out.write(reinterpret_cast<const char*>(&submeshCount), sizeof(submeshCount));

	for (const auto& sm : submeshes)
	{
		out.write(reinterpret_cast<const char*>(&sm.firstIndex), sizeof(sm.firstIndex));
		out.write(reinterpret_cast<const char*>(&sm.vertexOffset), sizeof(sm.vertexOffset));
		out.write(reinterpret_cast<const char*>(&sm.indexCount), sizeof(sm.indexCount));
		out.write(reinterpret_cast<const char*>(&sm.vertexCount), sizeof(sm.vertexCount));
		out.write(reinterpret_cast<const char*>(&sm.materialIndex), sizeof(sm.materialIndex));

		out.write(reinterpret_cast<const char*>(&sm.transform), sizeof(sm.transform));
		out.write(reinterpret_cast<const char*>(&sm.localTransform), sizeof(sm.localTransform));
		out.write(reinterpret_cast<const char*>(&sm.boundingBox.Min()), sizeof(Vector3f));
		out.write(reinterpret_cast<const char*>(&sm.boundingBox.Max()), sizeof(Vector3f));
	}

	out.close();

    return true;
}

static Ref<Mesh> DeserializeFromStream(std::istream& in)
{
	uint32_t magic = 0;
	in.read(reinterpret_cast<char*>(&magic), sizeof(magic));
	if (magic != MESH_MAGIC)
	{
		CLIENT_ERROR("Invalid mesh file format");
		return nullptr;
	}
	uint32_t version = 0;
	in.read(reinterpret_cast<char*>(&version), sizeof(version));
	if (version != MESH_VERSION)
	{
		CLIENT_ERROR("Unsupported mesh version: {0}", version);
		return nullptr;
	}
	// Read vertex and index count
	uint32_t vertexCount = 0, indexCount = 0;
	in.read(reinterpret_cast<char*>(&vertexCount), sizeof(vertexCount));
	std::vector<float> vertices(vertexCount);
	in.read(reinterpret_cast<char*>(vertices.data()), vertexCount * sizeof(float));
	in.read(reinterpret_cast<char*>(&indexCount), sizeof(indexCount));
	std::vector<uint32_t> indices(indexCount);
	in.read(reinterpret_cast<char*>(indices.data()), indexCount * sizeof(uint32_t));
	// Read submeshes
	uint32_t submeshCount = 0;
	in.read(reinterpret_cast<char*>(&submeshCount), sizeof(submeshCount));
	std::vector<Submesh> submeshes(submeshCount);
	
	for (auto& sm : submeshes)
	{
		in.read(reinterpret_cast<char*>(&sm.firstIndex), sizeof(sm.firstIndex));
		in.read(reinterpret_cast<char*>(&sm.vertexOffset), sizeof(sm.vertexOffset));
		in.read(reinterpret_cast<char*>(&sm.indexCount), sizeof(sm.indexCount));
		in.read(reinterpret_cast<char*>(&sm.vertexCount), sizeof(sm.vertexCount));
		in.read(reinterpret_cast<char*>(&sm.materialIndex), sizeof(sm.materialIndex));
		in.read(reinterpret_cast<char*>(&sm.transform), sizeof(sm.transform));
		in.read(reinterpret_cast<char*>(&sm.localTransform), sizeof(sm.localTransform));
		Vector3f min, max;
		in.read(reinterpret_cast<char*>(&min), sizeof(Vector3f));
		in.read(reinterpret_cast<char*>(&max), sizeof(Vector3f));
		sm.boundingBox = BoundingBox(min, max);
	}

	std::vector<Ref<Material>> materials; //TODO: Deserialize materials from file or use a default material

	return CreateRef<Mesh>(vertices, indices, submeshes, materials, s_StaticMeshLayout); //TODO: serialize buffer layout
}

Ref<Mesh> MeshSerializer::DeserializeFromMemory(const std::filesystem::path& filepath, const std::vector<uint8_t>& data)
{
	std::istringstream in(std::string(data.begin(), data.end()), std::ios::binary);
	return DeserializeFromStream(in);
}

Ref<Mesh> MeshSerializer::Deserialize(const std::filesystem::path& filepath)
{
	std::ifstream in(filepath, std::ios::binary);
	if (!in)
	{
		CLIENT_ERROR("Could not open mesh file {0}", filepath.string());
		return nullptr;
	}
	return DeserializeFromStream(in);
}
