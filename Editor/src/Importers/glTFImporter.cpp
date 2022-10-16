#include "glTFImporter.h"

#define TINYGLTF_IMPLEMENTATION
#include "tiny_gltf.h"

#include "Engine.h"

struct LoaderInfo {
	uint32_t* indexBuffer;
	Vertex* vertexBuffer;
	size_t indexPos = 0;
	size_t vertexPos = 0;
};

struct Node
{
	Node* parent;
	uint32_t index;
	std::vector<Node*> children;
	Matrix4x4 matrix;
	std::string name;
	Mesh* mesh;
	//Skin* skin;
	//int32_t skinIndex = -1;
	Vector3f translation;
	Vector3f scale = Vector3f(1.0f, 1.0f, 1.0f);
	Quaternion rotation;
	BoundingBox bvh;
	BoundingBox aabb;
};

void GetNodeProps(const tinygltf::Node& node, const tinygltf::Model& model, size_t& vertexCount, size_t& indexCount)
{
	if (node.children.size() > 0)
	{
		for (size_t i = 0; i < node.children.size(); i++)
		{
			GetNodeProps(model.nodes[node.children[i]], model, vertexCount, indexCount);
		}
	}
	if (node.mesh > -1)
	{
		const tinygltf::Mesh mesh = model.meshes[node.mesh];
		for (size_t i = 0; i < mesh.primitives.size(); i++)
		{
			auto primitive = mesh.primitives[i];
			vertexCount += model.accessors[primitive.attributes.find("POSITION")->second].count;
			if (primitive.indices > -1)
			{
				indexCount += model.accessors[primitive.indices].count;
			}
		}
	}
}

void LoadNode(Node* parent, const tinygltf::Node& node, uint32_t nodeIndex, const tinygltf::Model& model, LoaderInfo& loaderInfo, float globalScale)
{

}

void glTFImporter::ImportAssets(const std::filesystem::path& filepath, const std::filesystem::path& destination)
{
	LoaderInfo loaderInfo;

	float globalScale = 1.0f;

	tinygltf::Model gltfModel;
	tinygltf::TinyGLTF gltfContext;

	std::string error;
	std::string warning;

	bool binary = false;
	std::string filepathStr = filepath.string();
	size_t extpos = filepathStr.rfind('.', filepathStr.length());
	if (extpos != std::string::npos)
	{
		binary = (filepathStr.substr(extpos + 1, filepathStr.length() - extpos) == "glb");
	}

	bool fileLoaded = binary ?
		gltfContext.LoadBinaryFromFile(&gltfModel, &error, &warning, filepathStr.c_str()) : 
		gltfContext.LoadASCIIFromFile(&gltfModel, &error, &warning, filepathStr.c_str());

	if (!warning.empty())
	{
		CLIENT_WARN("Problem loading gltf: {0}", warning);
	}

	size_t vertexCount = 0;
	size_t indexCount = 0;

	if (fileLoaded)
	{
		const tinygltf::Scene& scene = gltfModel.scenes[gltfModel.defaultScene > -1 ? gltfModel.defaultScene : 0];

		for (size_t i = 0; i < scene.nodes.size(); i++)
		{
			GetNodeProps(gltfModel.nodes[scene.nodes[i]], gltfModel, vertexCount, indexCount);
		}
		loaderInfo.vertexBuffer = new Vertex[vertexCount];
		loaderInfo.indexBuffer = new uint32_t[indexCount];

		for (size_t i = 0; i < scene.nodes.size(); i++)
		{
			const tinygltf::Node node = gltfModel.nodes[scene.nodes[i]];
			LoadNode(nullptr, node, scene.nodes[i], gltfModel, loaderInfo, globalScale);
		}
		if (gltfModel.animations.size() > 0)
		{
			//LoadAnimations(gltfModel);
		}
		if (gltfModel.skins.size() > 0)
		{
			//LoadSkins(gltfModel);
		}

		/*for (auto node : linearNodes)
		{

		}*/
	}
	else
	{
		CLIENT_ERROR("Could not load gltf file: {0}", error);
	}

}
