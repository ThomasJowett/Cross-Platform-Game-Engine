#include "FbxImporter.h"

#include "Engine.h"

#include "ofbx.h"

void FbxImporter::ImportAssets(const std::filesystem::path& filepath, const std::filesystem::path& destination)
{
	PROFILE_FUNCTION();

	FILE* fp = fopen(filepath.string().c_str(), "rb");
	if (!fp)
	{
		CLIENT_ERROR("Could not open {0}", filepath.string());
		return;
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
		return;
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

			if (normals)
			{
				verticesList.push_back(normals[i].x);
				verticesList.push_back(normals[i].y);
				verticesList.push_back(normals[i].z);
			}
			else
			{
				verticesList.push_back(0.0f);
				verticesList.push_back(0.0f);
				verticesList.push_back(0.0f);
			}

			if (tangents)
			{
				verticesList.push_back(tangents[i].x);
				verticesList.push_back(tangents[i].y);
				verticesList.push_back(tangents[i].z);
			}
			else
			{
				verticesList.push_back(0.0f);
				verticesList.push_back(0.0f);
				verticesList.push_back(0.0f);
			}

			if (texcoords)
			{
				verticesList.push_back(texcoords[i].x);
				verticesList.push_back(texcoords[i].y);
			}
			else
			{
				verticesList.push_back(0.0f);
				verticesList.push_back(0.0f);
			}
		}

		std::vector<uint32_t> indicesList;

		// get the indices
		int indexCount = geom.getIndexCount();
		for (size_t i = 0; i < indexCount; i++)
		{
			int index = i * 3;
			indicesList.push_back(index);
			indicesList.push_back(index + 1);
			indicesList.push_back(index + 2);
		}


		std::string meshName = std::string(mesh.name);

		int numIndices = indicesList.size();

		std::filesystem::path outFilename = destination / meshName.substr(meshName.find_last_of("::") + 1);
		outFilename.replace_extension(".staticMesh");
		std::ofstream outbin(outFilename, std::ios::out | std::ios::binary);
		outbin.write((char*)&vertexCount, sizeof(uint32_t));
		outbin.write((char*)&numIndices, sizeof(uint32_t));
		outbin.write((char*)&verticesList[0], sizeof(float) * vertexCount * 11);
		outbin.write((char*)&indicesList[0], sizeof(uint32_t) * numIndices);
		outbin.close();
	}
	//TODO: import skeletons
	//TODO: import textures
	return;
}
