#include "FbxImporter.h"

#include "Engine.h"

#include "ofbx.h"

enum class Orientation
{
	Y_UP,
	Z_UP,
	Z_MINUS_UP,
	X_MINUS_UP,
	X_UP
};

struct ImportMesh
{
	const ofbx::Mesh* fbx;
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	BoundingBox aabb;
	int materialSlot;
	std::string name;
	bool isSkinned;
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
	bool is_valid = false;
};

struct ImportMaterial
{
	const ofbx::Material* fbx = nullptr;
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

int GetMaterialIndex(const ofbx::Mesh& mesh, const ofbx::Material& material)
{
	for (int i = 0; i < mesh.getMaterialCount(); ++i)
	{
		if (mesh.getMaterial(i) == &material) return i;
	}
	return -1;
}

void InsertHierarchy(std::vector<const ofbx::Object*>& bones, const ofbx::Object* node)
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
		InsertHierarchy(bones, parent);
		bones.push_back(node);
	}
}

Vector3f FixOrientation(const Vector3f& v, Orientation orientation)
{
	switch (orientation)
	{
	case Orientation::Y_UP:
		return Vector3f(v.x, v.y, v.z);
	case Orientation::Z_UP:
		return Vector3f(v.x, v.z, -v.y);
	case Orientation::Z_MINUS_UP:
		return Vector3f(v.x, -v.z, v.y);
	case Orientation::X_MINUS_UP:
		return Vector3f(v.y, -v.x, v.z);
	case Orientation::X_UP:
		return Vector3f(-v.y, v.x, v.z);
	default:
		return Vector3f(v.x, v.y, v.z);
	}
}

ofbx::Vec3 operator-(const ofbx::Vec3& a, const ofbx::Vec3& b)
{
	return { a.x - b.x, a.y - b.y, a.z - b.z };
}

ofbx::Vec2 operator-(const ofbx::Vec2& a, const ofbx::Vec2& b)
{
	return { a.x - b.x, a.y - b.y };
}

void ComputeTangents(ofbx::Vec3* out, uint32_t vertexCount, const ofbx::Vec3* vertices, const ofbx::Vec3* normals, const ofbx::Vec2* uvs)
{
	for (uint32_t i = 0; i < vertexCount; i += 3)
	{
		const ofbx::Vec3 v0 = vertices[i + 0];
		const ofbx::Vec3 v1 = vertices[i + 1];
		const ofbx::Vec3 v2 = vertices[i + 2];
		const ofbx::Vec2 uv0 = uvs[i + 0];
		const ofbx::Vec2 uv1 = uvs[i + 1];
		const ofbx::Vec2 uv2 = uvs[i + 2];

		const ofbx::Vec3 dv10 = v1 - v0;
		const ofbx::Vec3 dv20 = v2 - v0;
		const ofbx::Vec2 duv10 = uv1 - uv0;
		const ofbx::Vec2 duv20 = uv2 - uv0;

		const float dir = duv20.x * duv10.y - duv20.y * duv10.x < 0 ? -1.0f : 1.0f;
		ofbx::Vec3 tangent;
		tangent.x = (dv20.x * duv10.y - dv10.x * duv20.y) * dir;
		tangent.y = (dv20.y * duv10.y - dv10.y * duv20.y) * dir;
		tangent.z = (dv20.z * duv10.y - dv10.z * duv20.y) * dir;
		const float l = 1 / sqrtf(float(tangent.x * tangent.x + tangent.y * tangent.y + tangent.z * tangent.z));
		tangent.x *= l;
		tangent.y *= l;
		tangent.z *= l;
		out[i + 0] = tangent;
		out[i + 1] = tangent;
		out[i + 2] = tangent;
	}
}

ImportMesh LoadMesh(const ofbx::Mesh* fbxMesh, uint32_t triangleStart, uint32_t triangleEnd)
{
	const uint32_t firstVertexOffset = triangleStart * 3;
	const uint32_t lastVertexOffset = triangleEnd * 3;
	const int vertexCount = lastVertexOffset - firstVertexOffset + 3;

	const ofbx::Geometry* geom = fbxMesh->getGeometry();

	const ofbx::Vec3* vertices = geom->getVertices();
	const ofbx::Vec3* normals = geom->getNormals();
	const ofbx::Vec3* tangents = geom->getTangents();
	const ofbx::Vec2* texcoords = geom->getUVs();

	if (!tangents && texcoords)
	{
		ofbx::Vec3* generatedTangents = new ofbx::Vec3[vertexCount];
		ComputeTangents(generatedTangents, vertexCount, vertices, normals, texcoords);
		tangents = generatedTangents;
	}

	ImportMesh importMesh;

	importMesh.fbx = fbxMesh;
	importMesh.vertices.resize(vertexCount);
	ofbx::Matrix transform = fbxMesh->getGlobalTransform();

	Matrix4x4 transformf(
		(float)transform.m[0], (float)transform.m[4], (float)transform.m[8], (float)transform.m[12],
		(float)transform.m[1], (float)transform.m[5], (float)transform.m[9], (float)transform.m[13],
		(float)transform.m[2], (float)transform.m[6], (float)transform.m[10], (float)transform.m[14],
		(float)transform.m[3], (float)transform.m[7], (float)transform.m[11], (float)transform.m[15]
	);

	for (int i = 0; i < vertexCount; i++)
	{
		Vertex& vertex = importMesh.vertices[i];

		vertex.position = transformf * Vector3f((float)vertices[i].x, (float)vertices[i].y, (float)vertices[i].z);

		importMesh.aabb.Merge(vertex.position);

		if (normals)
			vertex.normal = Vector3f((float)normals[i].x, (float)normals[i].y, (float)normals[i].z).GetNormalized();
		if (tangents)
			vertex.tangent = Vector3f((float)tangents[i].x, (float)tangents[i].y, (float)tangents[i].z).GetNormalized();
		if (texcoords)
			vertex.texcoord = Vector2f((float)texcoords[i].x, (float)texcoords[i].y);
	}

	// get the indices
	//uint32_t indexCount = (uint32_t)geom->getIndexCount();
	//const int* indices = geom->getFaceIndices();
	for (int i = 0; i < vertexCount / 3; i++)
	{
		uint32_t index = i * 3;
		importMesh.indices.push_back(index);
		importMesh.indices.push_back(index + 1);
		importMesh.indices.push_back(index + 2);
	}

	if (fbxMesh->getMaterialCount() > 0)
	{
		if (geom->getMaterials())
			importMesh.materialSlot = geom->getMaterials()[triangleStart];
		else
			importMesh.materialSlot = 0;
	}
	else
	{
		importMesh.materialSlot = -1;
	}

	importMesh.name = fbxMesh->name;
	importMesh.name = importMesh.name.substr(importMesh.name.find_last_of("::") + 1);
	return importMesh;
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

	Orientation orientation;

	float m_fbx_scale = scene->getGlobalSettings()->UnitScaleFactor * 0.01f;

	const ofbx::GlobalSettings* settings = scene->getGlobalSettings();

	switch (settings->UpAxis)
	{
	case ofbx::UpVector_AxisX: orientation = Orientation::X_UP; break;
	case ofbx::UpVector_AxisY: orientation = Orientation::Y_UP; break;
	case ofbx::UpVector_AxisZ: orientation = Orientation::Z_UP; break;
	}

	std::vector<ImportMesh> meshes;
	std::vector<ImportMaterial> materials;
	std::vector<ImportAnimation> animations;
	std::vector<const ofbx::Object*> bones;
	std::unordered_map<const ofbx::Material*, std::string> materialNameMap;

	std::string assetDirectory = filepath.string();
	assetDirectory = assetDirectory.substr(0, assetDirectory.find_last_of(std::filesystem::path::preferred_separator));

	for (int i = 0, count = scene->getMeshCount(); i < count; ++i)
	{
		const ofbx::Mesh* fbxMesh = scene->getMesh(i);
		const ofbx::Geometry* geom = fbxMesh->getGeometry();
		const int triangleCount = geom->getVertexCount() / 3;

		if (triangleCount <= 0)
			continue;

		if (fbxMesh->getMaterialCount() < 2 || !geom->getMaterials())
		{
			meshes.push_back(LoadMesh(fbxMesh, 0, triangleCount - 1));
		}
		else
		{
			const int* materials = geom->getMaterials();
			int rangeStart = 0;
			int rangeStartMaterial = materials[rangeStart];
			for (int triangleIndex = 1; triangleIndex < triangleCount; triangleIndex++)
			{
				if (rangeStartMaterial != materials[triangleIndex])
				{
					meshes.push_back(LoadMesh(fbxMesh, rangeStart, triangleIndex - 1));

					rangeStart = triangleIndex;
					rangeStartMaterial = materials[triangleIndex];
				}
			}
			meshes.push_back(LoadMesh(fbxMesh, rangeStart, triangleCount - 1));
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
		if (mesh.materialSlot == -1) continue;
		const ofbx::Material* fbx_mat = mesh.fbx->getMaterial(mesh.materialSlot);
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
			char path[260];
			filename.toString(path);
			tex.src = std::filesystem::absolute(std::filesystem::path(assetDirectory) / path);
			tex.is_valid = (std::filesystem::exists(tex.src) && std::filesystem::is_regular_file(tex.src));

			if (!tex.is_valid)
			{
				CLIENT_ERROR("Could not import texture from fbx: {0}", tex.src.string());
			}
			else
			{
				// Copy the texture into the destination folder
				tex.path = destination / tex.src.filename();
				if (!std::filesystem::exists(tex.path))
					std::filesystem::copy_file(tex.src, tex.path);
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

		name = name.substr(name.find_last_of("::") + 1);

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
		if (mesh.isSkinned)
		{
			if (mesh.fbx->getGeometry())
			{
				const ofbx::Skin* skin = mesh.fbx->getGeometry()->getSkin();
				if (skin)
				{
					for (int i = 0; i < skin->getClusterCount(); ++i)
					{
						const ofbx::Cluster* cluster = skin->getCluster(i);
						InsertHierarchy(bones, cluster->getLink());
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
					InsertHierarchy(bones, node->getBone());
				}
			}
		}
	}

	std::sort(bones.begin(), bones.end());
	bones.erase(std::unique(bones.begin(), bones.end()), bones.end());

	// write materials
	for (ImportMaterial& material : materials)
	{
		const std::string materialName = materialNameMap[material.fbx];

		std::filesystem::path materialPath = destination / materialName;
		materialPath.replace_extension(".material");
		if (std::filesystem::exists(materialPath))
			continue;

		Material outputMaterial;

		ofbx::Color diffuseColour = material.fbx->getDiffuseColor();

		outputMaterial.SetTint(Colour(diffuseColour.r, diffuseColour.g, diffuseColour.b, 1.0f));

		if (material.textures[0].is_valid)
		{
			Ref<Texture2D> albedo = Texture2D::Create(material.textures[0].path);
			outputMaterial.AddTexture(albedo, 0);
		}

		if (material.textures[1].is_valid)
		{
			Ref<Texture2D> normalMap = Texture2D::Create(material.textures[1].path);
			outputMaterial.AddTexture(normalMap, 1);
		}

		if (material.textures[2].is_valid)
		{
			Ref<Texture2D> mixMap = Texture2D::Create(material.textures[2].path);
			outputMaterial.AddTexture(mixMap, 2);
		}

		outputMaterial.SetShader("Standard");

		outputMaterial.SaveMaterial(materialPath);
	}

	// write mesh
	std::filesystem::path outFilename = destination / filepath.filename();
	outFilename.replace_extension(".staticmesh");

	std::ofstream outbin(outFilename, std::ios::out | std::ios::binary);

	size_t meshCount = meshes.size();
	outbin.write((char*)&meshCount, sizeof(size_t));

	for (ImportMesh& mesh : meshes)
	{
		size_t numVertices = mesh.vertices.size();
		size_t numIndices = mesh.indices.size();

		std::string materialName = mesh.materialSlot != -1 ? materialNameMap[mesh.fbx->getMaterial(mesh.materialSlot)] : "";

		outbin << materialName.size();
		outbin << materialName;

		outbin.write((char*)&numVertices, sizeof(size_t));
		outbin.write((char*)&numIndices, sizeof(size_t));

		outbin.write((char*)&mesh.vertices[0], sizeof(Vertex) * numVertices);
		outbin.write((char*)&mesh.indices[0], sizeof(uint32_t) * numIndices);
	}
	outbin.close();

	//TODO: import skeletons
	//TODO: import textures
	return;
}