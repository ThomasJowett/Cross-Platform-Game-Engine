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
	Node* newNode = new Node{};
	newNode->index = nodeIndex;
	newNode->parent = parent;
	newNode->name = node.name;
	//newNode->skinIndex = node.skin;

	// Generate local node matrix
	if (node.translation.size() == 3)
	{
		newNode->translation = Vector3f((float)node.translation[0], (float)node.translation[1], (float)node.translation[2]);
	}
	if (node.rotation.size() == 4)
	{
		newNode->rotation = Quaternion((float)node.rotation[0], (float)node.rotation[1], (float)node.rotation[2], (float)node.rotation[3]);
	}
	if (node.scale.size() == 3)
	{
		newNode->scale = Vector3f((float)node.scale[0], (float)node.scale[1], (float)node.scale[2]);
	}
	if (node.matrix.size() == 16)
	{
		newNode->matrix  = Matrix4x4(
			(float)node.matrix[0], (float)node.matrix[4], (float)node.matrix[8], (float)node.matrix[12],
			(float)node.matrix[1], (float)node.matrix[5], (float)node.matrix[9], (float)node.matrix[13],
			(float)node.matrix[2], (float)node.matrix[6], (float)node.matrix[10], (float)node.matrix[14],
			(float)node.matrix[3], (float)node.matrix[7], (float)node.matrix[11], (float)node.matrix[15]
		);
	}

	if (node.children.size() > 0)
	{
		for (size_t i = 0; i < node.children.size(); i++)
		{
			LoadNode(newNode, model.nodes[node.children[i]], node.children[i], model, loaderInfo, globalScale);
		}
	}


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
