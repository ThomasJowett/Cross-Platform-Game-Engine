#include "FbxImporter.h"

#include "Engine.h"

#include "ofbx.h"

struct ImportMesh
{
	const ofbx::Mesh* fbx = nullptr;
	const ofbx::Material* fbx_mat = nullptr;
	bool is_skinned = false;
	int bone_idx = -1;
	int submesh;
	AABB aabb;
	Matrix4x4 transform_matrix;
	Vector3f origin;
};

struct ImportGeometry
{
	const ofbx::Geometry* fbx = nullptr;
	std::vector<uint32_t> indices;

};

struct ImportAnimation
{
	const ofbx::AnimationStack* fbx = nullptr;
	const ofbx::IScene* scene;
	bool import = true;
	std::string name;
};

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
	bool is_valid = true;
};

struct ImportMaterial
{
	const ofbx::Material* fbx = nullptr;
	bool import = true;
	ImportTexture textures[ImportTexture::Count];

	bool operator<(const ImportMaterial& other)const
	{
		return fbx < other.fbx;
	}

	bool operator==(const ImportMaterial& other)const
	{
		return fbx == other.fbx;
	}
};

int getMaterialIndex(const ofbx::Mesh& mesh, const ofbx::Material& material)
{
	for (int i = 0; i < mesh.getMaterialCount(); ++i)
	{
		if (mesh.getMaterial(i) == &material) return i;
	}
	return -1;
}

void insertHierarchy(std::vector<const ofbx::Object*>& bones, const ofbx::Object* node)
{
	if (!node) return;
	bool contains = false;
	for (const ofbx::Object* bone : bones)
	{
		if (bone == node)
		{
			contains = true;
			break;
		}
	}
	if (contains)
	{
		ofbx::Object* parent = node->getParent();
		insertHierarchy(bones, parent);
		bones.push_back(node);
	}
}

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

	ofbx::IScene* scene = ofbx::load((ofbx::u8*)content, file_size, (ofbx::u64)ofbx::LoadFlags::TRIANGULATE);

	if (!scene)
	{
		CLIENT_ERROR("Could not import fbx {0}: {1}", filepath.string(), ofbx::getError());
		return;
	}

	enum class Orientation
	{
		Y_UP,
		Z_UP,
		Z_MINUS_UP,
		X_MINUS_UP,
		X_UP
	}orientation;

	float m_fbx_scale = scene->getGlobalSettings()->UnitScaleFactor * 0.01f;

	const ofbx::GlobalSettings* settings = scene->getGlobalSettings();

	switch (settings->UpAxis)
	{
	case ofbx::UpVector_AxisX: orientation = Orientation::X_UP; break;
	case ofbx::UpVector_AxisY: orientation = Orientation::Y_UP; break;
	case ofbx::UpVector_AxisZ: orientation = Orientation::Z_UP; break;
	}

	std::vector<ImportGeometry> geometries;
	std::vector<ImportMesh> meshes;
	std::vector<ImportMaterial> materials;
	std::vector<ImportAnimation> animations;
	std::vector<const ofbx::Object*> bones;
	std::unordered_map<const ofbx::Material*, std::string> materialNameMap;

	//gather the geometries
	for (int i = 0, count = scene->getGeometryCount(); i < count; ++i)
	{
		ImportGeometry geom;
		geom.fbx = scene->getGeometry(i);
		geometries.push_back(geom);
	}

	// gather the meshes
	for (int i = 0, count = scene->getMeshCount(); i < count; ++i)
	{
		const ofbx::Mesh* fbx_mesh = scene->getMesh(i);
		const int materialCount = fbx_mesh->getMaterialCount();
		for (int j = 0; j < materialCount; ++j)
		{
			ImportMesh mesh;
			if (fbx_mesh->getGeometry() && fbx_mesh->getGeometry()->getSkin())
			{
				const ofbx::Skin* skin = fbx_mesh->getGeometry()->getSkin();
				for (int k = 0; k < skin->getClusterCount(); ++k)
				{
					if (skin->getCluster(k)->getIndicesCount() > 0)
					{
						mesh.is_skinned = true;
						break;
					}
				}
			}
			mesh.fbx = fbx_mesh;
			mesh.fbx_mat = fbx_mesh->getMaterial(j);
			mesh.submesh = materialCount > 1 ? j : -1;
			meshes.push_back(mesh);
		}
	}

	// gather animations
	for (int i = 0, count = scene->getAnimationStackCount(); i < count; ++i)
	{
		ImportAnimation anim;
		anim.scene = scene;
		anim.fbx = scene->getAnimationStack(i);
		const ofbx::TakeInfo* take_info = scene->getTakeInfo(anim.fbx->name);
		if (take_info)
		{
		}
		else
		{
			anim.name = "";
		}
		animations.push_back(anim);
	}

	if (animations.size() == 1)
	{
		animations[0].name = "";
	}

	// gather materials
	for (ImportMesh& mesh : meshes)
	{
		const ofbx::Material* fbx_mat = mesh.fbx_mat;
		if (!fbx_mat) continue;

		ImportMaterial mat;
		mat.fbx = fbx_mat;

		for (uint8 i = 0; i < ImportTexture::Count; i++)
		{
			const ofbx::Texture* texture = mat.fbx->getTexture((ofbx::Texture::TextureType)i);
			if (!texture) continue;

			ImportTexture& tex = mat.textures[i];
			tex.fbx = texture;
			ofbx::DataView filename = tex.fbx->getRelativeFileName();
			if (filename == "")
				filename = tex.fbx->getFileName();
			char path[MAX_PATH];
			filename.toString(path);
			tex.src = tex.path = path;
			tex.is_valid = std::filesystem::exists(tex.src);

			if (!tex.is_valid)
			{
				CLIENT_ERROR("Could not import texture from fbx: {0}", tex.src.string());
			}
		}

		materials.push_back(mat);
	}

	std::vector<std::string> names;
	for (ImportMaterial& mat : materials)
	{
		if (materialNameMap.find(mat.fbx) != materialNameMap.end())
		{
			continue;
		}

		std::string name = mat.fbx->name;

		std::replace_if(name.begin(), name.end(), [](char c) {return !std::isalnum(c); }, '_');

		if (std::find(names.begin(), names.end(), name) != names.end())
		{
			std::string originalName = name;

			uint32_t number = 1;
			do {
				name = originalName;
				name += "_(" + std::to_string(number) + ')';
				number++;
			} while (std::find(names.begin(), names.end(), name) != names.end());
		}
		names.push_back(name);
		materialNameMap[mat.fbx] = name;
	}

	std::sort(materials.begin(), materials.end());
	materials.erase(std::unique(materials.begin(), materials.end()), materials.end());

	// gather bones
	bool any_skinned = false;
	for (const ImportMesh& mesh : meshes)
	{
		if (mesh.is_skinned)
		{
			if (mesh.fbx->getGeometry())
			{
				const ofbx::Skin* skin = mesh.fbx->getGeometry()->getSkin();
				if (skin)
				{
					for (int i = 0; i < skin->getClusterCount(); ++i)
					{
						const ofbx::Cluster* cluster = skin->getCluster(i);
						insertHierarchy(bones, cluster->getLink());
					}
				}
			}
		}
	}

	for (int i = 0, n = scene->getAnimationStackCount(); i < n; ++i)
	{
		const ofbx::AnimationStack* stack = scene->getAnimationStack(i);
		for (int j = 0; stack->getLayer(j); ++j)
		{
			const ofbx::AnimationLayer* layer = stack->getLayer(j);
			for (int k = 0; layer->getCurveNode(k); ++k)
			{
				const ofbx::AnimationCurveNode* node = layer->getCurveNode(k);
				if (node->getBone())
				{
					insertHierarchy(bones, node->getBone());
				}
			}
		}
	}

	std::sort(bones.begin(), bones.end());
	bones.erase(std::unique(bones.begin(), bones.end()), bones.end());

	for (auto& geometry : geometries)
	{
		const ofbx::Geometry* geom = geometry.fbx;

		// get the vertices
		uint32_t vertexCount = geom->getVertexCount();
		const ofbx::Vec3* vertices = geom->getVertices();
		const ofbx::Vec3* normals = geom->getNormals();
		const ofbx::Vec3* tangents = geom->getTangents();
		const ofbx::Vec2* texcoords = geom->getUVs();
		const int* geom_materials = geom->getMaterials();

		//int material_idx = getMaterialIndex(mesh, )

		std::vector<float> verticesList;

		for (uint32_t j = 0; j < vertexCount; j++)
		{
			//if (geom_materials && geom_materials[j / 3] != mat)
			verticesList.push_back((float)vertices[j].x);
			verticesList.push_back((float)vertices[j].y);
			verticesList.push_back((float)vertices[j].z);

			if (normals)
			{
				verticesList.push_back((float)normals[j].x);
				verticesList.push_back((float)normals[j].y);
				verticesList.push_back((float)normals[j].z);
			}
			else
			{
				verticesList.push_back(0.0f);
				verticesList.push_back(0.0f);
				verticesList.push_back(0.0f);
			}

			if (tangents)
			{
				verticesList.push_back((float)tangents[j].x);
				verticesList.push_back((float)tangents[j].y);
				verticesList.push_back((float)tangents[j].z);
			}
			else
			{
				verticesList.push_back(0.0f);
				verticesList.push_back(0.0f);
				verticesList.push_back(0.0f);
			}

			if (texcoords)
			{
				verticesList.push_back((float)texcoords[j].x);
				verticesList.push_back((float)texcoords[j].y);
			}
			else
			{
				verticesList.push_back(0.0f);
				verticesList.push_back(0.0f);
			}
		}

		std::vector<uint32_t> indicesList;

		// get the indices
		uint32_t indexCount = (uint32_t)geom->getIndexCount();
		for (uint32_t j = 0; j < indexCount; j++)
		{
			uint32_t index = j * 3;
			indicesList.push_back(index);
			indicesList.push_back(index + 1);
			indicesList.push_back(index + 2);
		}

		//std::string meshName = std::string(mesh->name);
		std::string meshName = "Test";

		size_t numIndices = indicesList.size();

		std::filesystem::path outFilename = destination / meshName.substr(meshName.find_last_of("::") + 1);
		outFilename.replace_extension(".staticmesh");
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
