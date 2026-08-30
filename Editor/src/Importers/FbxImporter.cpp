#include "FbxImporter.h"

#include "Renderer/Mesh.h"
#include "Core/BoundingBox.h"
#include "math/Matrix.h"
#include "Asset/Material.h"
#include "Logging/Logger.h"
#include "Core/Colour.h"
#include "Utilities/MeshSerializer.h"
#include "Utilities/GeometryGenerator.h"

#include <cfloat>

#include "ofbx.h"

namespace fbx_importer_internal {

struct ImportTexture
{
	enum Type
	{
		Diffuse,
		Normal,
		Specular,
		Count
	};

	const ofbx::Texture* fbx = nullptr;
	std::filesystem::path path;
	std::filesystem::path src;
	bool is_valid = false;
};

struct ImportMaterial
{
	const ofbx::Material* fbx = nullptr;
	ImportTexture textures[ImportTexture::Count];
	std::string name;

	bool operator<(const ImportMaterial& other)const
	{
		return fbx < other.fbx;
	}

	bool operator==(const ImportMaterial& other)const
	{
		return fbx == other.fbx;
	}
};

} // namespace fbx_importer_internal

using namespace fbx_importer_internal;

void FbxImporter::ImportAssets(const std::filesystem::path& filepath, const std::filesystem::path& destination)
{
	PROFILE_FUNCTION();

	long file_size;
	ofbx::u8* content;

	std::ifstream file(filepath, std::ios::in | std::ios::binary);
	if (file.is_open())
	{
		file.seekg(0, std::ios::end);
		file_size = (long)file.tellg();
		content = new ofbx::u8[file_size];
		file.seekg(0, std::ios::beg);
		file.read((char*)&content[0], file_size);
		file.close();
	}
	else
	{
		CLIENT_ERROR("Could not open {0}", filepath.string());
		return;
	}

	ofbx::IScene* scene = ofbx::load((ofbx::u8*)content, (ofbx::usize)file_size, (ofbx::u16)ofbx::LoadFlags::NONE);

	if (!scene)
	{
		CLIENT_ERROR("Could not import fbx {0}: {1}", filepath.string(), ofbx::getError());
		delete[] content;
		return;
	}

	float scale = scene->getGlobalSettings()->UnitScaleFactor * 0.01f;

	std::string assetDirectory = filepath.parent_path().string();

	// Process materials
	std::vector<ImportMaterial> materials;
	std::unordered_map<const ofbx::Material*, uint32_t> materialIndexMap;

	for (int i = 0; i < scene->getAllObjectCount(); ++i)
	{
		const ofbx::Object* obj = scene->getAllObjects()[i];
		if (obj->getType() == ofbx::Object::Type::MATERIAL)
		{
			const ofbx::Material* fbx_mat = static_cast<const ofbx::Material*>(obj);
			ImportMaterial mat;
			mat.fbx = fbx_mat;
			mat.name = fbx_mat->name;
			std::replace_if(mat.name.begin(), mat.name.end(), [](char c) {return !std::isalnum(c); }, '_');
			
			for (int t = 0; t < ImportTexture::Count; ++t)
			{
				const ofbx::Texture* texture = fbx_mat->getTexture((ofbx::Texture::TextureType)t);
				if (texture)
				{
					ImportTexture& tex = mat.textures[t];
					tex.fbx = texture;
					ofbx::DataView filename = tex.fbx->getRelativeFileName();
					if (filename == "") filename = tex.fbx->getFileName();
					char path_buf[260];
					filename.toString(path_buf);
					tex.src = std::filesystem::path(assetDirectory) / path_buf;
					if (std::filesystem::exists(tex.src))
					{
						tex.path = destination / tex.src.filename();
						if (!std::filesystem::exists(tex.path))
							std::filesystem::copy_file(tex.src, tex.path);
						tex.is_valid = true;
					}
				}
			}
			materialIndexMap[fbx_mat] = (uint32_t)materials.size();
			materials.push_back(mat);
		}
	}

	// Process meshes
	for (int i = 0; i < scene->getMeshCount(); ++i)
	{
		const ofbx::Mesh* fbxMesh = scene->getMesh(i);
		const ofbx::Geometry* geom = fbxMesh->getGeometry();
		const ofbx::GeometryData& geomData = geom->getGeometryData();

		ofbx::Vec3Attributes positions = geomData.getPositions();
		ofbx::Vec3Attributes normals = geomData.getNormals();
		ofbx::Vec3Attributes tangents = geomData.getTangents();
		ofbx::Vec2Attributes texcoords = geomData.getUVs();

		std::vector<Vertex> allVertices;
		std::vector<uint32_t> allIndices;
		std::vector<Submesh> submeshes;
		std::vector<Ref<Material>> engineMaterials;

		ofbx::DMatrix transform = fbxMesh->getGlobalTransform();
		Matrix4x4 transformf(
			(float)transform.m[0], (float)transform.m[4], (float)transform.m[8], (float)transform.m[12],
			(float)transform.m[1], (float)transform.m[5], (float)transform.m[9], (float)transform.m[13],
			(float)transform.m[2], (float)transform.m[6], (float)transform.m[10], (float)transform.m[14],
			(float)transform.m[3], (float)transform.m[7], (float)transform.m[11], (float)transform.m[15]
		);

		for (int p = 0; p < geomData.getPartitionCount(); ++p)
		{
			ofbx::GeometryPartition partition = geomData.getPartition(p);
			if (partition.triangles_count <= 0) continue;

			Submesh submesh;
			submesh.firstIndex = (uint32_t)allIndices.size();
			submesh.vertexOffset = (uint32_t)allVertices.size();
			submesh.indexCount = partition.triangles_count * 3;
			submesh.vertexCount = partition.triangles_count * 3;
			// Vertex positions below are already baked into world space via transformf, and
			// Renderer::Submit combines submesh.transform with the entity's own transform at draw
			// time - storing transformf here too would apply it a second time.
			submesh.transform = Matrix4x4();
			submesh.localTransform = Matrix4x4();

			const ofbx::Material* fbx_mat = fbxMesh->getMaterialCount() > 0 ? fbxMesh->getMaterial(p) : nullptr;
			if (fbx_mat && materialIndexMap.count(fbx_mat))
			{
				submesh.materialIndex = (uint32_t)engineMaterials.size();
				
				const ImportMaterial& imat = materials[materialIndexMap[fbx_mat]];
				std::filesystem::path matPath = destination / (imat.name + ".material");
				Ref<Material> engineMat;
				if (std::filesystem::exists(matPath))
				{
					engineMat = CreateRef<Material>(matPath);
				}
				else
				{
					engineMat = CreateRef<Material>();
					engineMat->SetShader("Standard");
					ofbx::Color c = imat.fbx->getDiffuseColor();
					engineMat->SetTint(Colour(c.r, c.g, c.b, 1.0f));
					if (imat.textures[ImportTexture::Diffuse].is_valid)
						engineMat->AddTexture(Texture2D::Create(imat.textures[ImportTexture::Diffuse].path), 0);
					if (imat.textures[ImportTexture::Normal].is_valid)
						engineMat->AddTexture(Texture2D::Create(imat.textures[ImportTexture::Normal].path), 1);
					if (imat.textures[ImportTexture::Specular].is_valid)
						engineMat->AddTexture(Texture2D::Create(imat.textures[ImportTexture::Specular].path), 2);
					engineMat->SaveMaterial(matPath);
				}
				engineMaterials.push_back(engineMat);
			}
			else
			{
				submesh.materialIndex = (uint32_t)engineMaterials.size();
				engineMaterials.push_back(Material::GetDefaultMaterial());
			}

			Vector3f subMin(FLT_MAX, FLT_MAX, FLT_MAX), subMax(-FLT_MAX, -FLT_MAX, -FLT_MAX);

			for (int polyIdx = 0; polyIdx < partition.polygon_count; ++polyIdx)
			{
				ofbx::GeometryPartition::Polygon polygon = partition.polygons[polyIdx];
				int tri_indices[128];
				int num_indices = ofbx::triangulate(geomData, polygon, tri_indices);
				for (int k = 0; k < num_indices; ++k)
				{
					int idx = tri_indices[k];
					Vertex v;
					ofbx::Vec3 p = positions.get(idx);
					v.position = transformf * (Vector3f((float)p.x, (float)p.y, (float)p.z) * scale);
					
					subMin.x = std::min(subMin.x, v.position.x); subMin.y = std::min(subMin.y, v.position.y); subMin.z = std::min(subMin.z, v.position.z);
					subMax.x = std::max(subMax.x, v.position.x); subMax.y = std::max(subMax.y, v.position.y); subMax.z = std::max(subMax.z, v.position.z);

					if (normals.values) { ofbx::Vec3 n = normals.get(idx); v.normal = Vector3f((float)n.x, (float)n.y, (float)n.z).GetNormalized(); }
					if (tangents.values) { ofbx::Vec3 t = tangents.get(idx); v.tangent = Vector3f((float)t.x, (float)t.y, (float)t.z).GetNormalized(); }
					if (texcoords.values) { ofbx::Vec2 uv = texcoords.get(idx); v.texcoord = Vector2f((float)uv.x, (float)uv.y); }
					
					allIndices.push_back((uint32_t)allVertices.size());
					allVertices.push_back(v);
				}
			}
			submesh.SetBoundingBox(subMin, subMax);
			submeshes.push_back(submesh);
		}

		if (!allVertices.empty())
		{
			if (!tangents.values && texcoords.values)
				GeometryGenerator::GenerateTangents(allVertices.data(), (uint32_t)allVertices.size());

			Ref<Mesh> mesh = CreateRef<Mesh>(GeometryGenerator::FlattenVertices(allVertices), allIndices, submeshes, engineMaterials, s_StaticMeshLayout);
			std::string meshName = fbxMesh->name;
			meshName = meshName.substr(meshName.find_last_of("::") + 1);
			std::replace_if(meshName.begin(), meshName.end(), [](char c) {return !std::isalnum(c); }, '_');
			std::filesystem::path outPath = destination / (meshName + ".staticmesh");
			MeshSerializer::Serialize(outPath, mesh, MeshLayout::StaticMesh);
		}
	}

	scene->destroy();
	delete[] content;
}
