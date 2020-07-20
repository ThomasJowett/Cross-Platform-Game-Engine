#include "FbxImporter.h"

Ref<VertexArray> Importer::Fbx::ImportStaticMesh(const std::filesystem::path& filepath)
{
	PROFILE_FUNCTION();

	Ref<VertexArray> returnModel = VertexArray::Create();

	FILE* fp = fopen(filepath.string().c_str(), "rb");
	if (!fp)
	{
		CLIENT_ERROR("Could not open {0}", filepath.string());
		return returnModel;
	}

	fseek(fp, 0, SEEK_END);
	long file_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	auto* content = new ofbx::u8[file_size];
	fread(content, 1, file_size, fp);

	ofbx::IScene* scene = ofbx::load((ofbx::u8*)content, file_size, (ofbx::u64)ofbx::LoadFlags::TRIANGULATE);

	if (!scene)
	{
		CLIENT_ERROR(ofbx::getError());
		return returnModel;
	}

	for (size_t i = 0; i < scene->getMeshCount(); i++)
	{
		const ofbx::Mesh& mesh = *scene->getMesh(i);

		const ofbx::Geometry& geom = *mesh.getGeometry();

		// get the vertices
		int vertexCount = geom.getVertexCount();
		const ofbx::Vec3* vertices = geom.getVertices();
		const ofbx::Vec3* normals = geom.getNormals();
		const ofbx::Vec3* tangents = geom.getTangents();
		const ofbx::Vec2* texcoords = geom.getUVs();

		std::vector<float> verticesList;

		for (size_t i = 0; i < vertexCount; i++)
		{
			verticesList.push_back(vertices[i].x);
			verticesList.push_back(vertices[i].y);
			verticesList.push_back(vertices[i].z);

			verticesList.push_back(normals[i].x);
			verticesList.push_back(normals[i].y);
			verticesList.push_back(normals[i].z);

			verticesList.push_back(tangents[i].x);
			verticesList.push_back(tangents[i].y);
			verticesList.push_back(tangents[i].z);

			verticesList.push_back(texcoords[i].x);
			verticesList.push_back(texcoords[i].y);
		}

		std::vector<uint32_t> indicesList;

		// get the indices
		int indexCount = geom.getIndexCount();
		for (size_t i = 0; i < indexCount; i++)
		{
			int sixi = i * 6;
			indicesList.push_back(sixi);
			indicesList.push_back(sixi + 1);
			indicesList.push_back(sixi + 2);
			indicesList.push_back(sixi);
			indicesList.push_back(sixi + 2);
			indicesList.push_back(sixi + 3);
		}

	}

	//TODO: create the vertex array
	return returnModel;
}
