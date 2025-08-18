#include "glTFImporter.h"

#include "Utilities/MeshSerializer.h"
#include "Utilities/GeometryGenerator.h"

#define TINYGLTF_IMPLEMENTATION
#include "tiny_gltf.h"

#include "Engine.h"

struct LoaderInfo {
	std::vector<uint32_t> indexBuffer;
	std::vector<Vertex> vertexBuffer;
	size_t indexPos = 0;
	size_t vertexPos = 0;
	std::filesystem::path destination;
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
	if (!node.children.empty())
	{
		for (auto& child : node.children)
		{
			GetNodeProps(model.nodes[child], model, vertexCount, indexCount);
		}
	}
	if (node.mesh > -1)
	{
		const tinygltf::Mesh mesh = model.meshes[node.mesh];
		for (auto& primitive : mesh.primitives)
		{
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
	PROFILE_FUNCTION();

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
		newNode->matrix = Matrix4x4(
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

	if (node.mesh > -1)
	{
		const tinygltf::Mesh mesh = model.meshes[node.mesh];

		std::vector<Submesh> submeshes;
		std::vector<Ref<Material>> materials;

		bool hasSkin = false;

		size_t vertexStart = loaderInfo.vertexPos;
		size_t indexStart = loaderInfo.indexPos;

		for (size_t j = 0; j < mesh.primitives.size(); j++)
		{
			const tinygltf::Primitive& primitive = mesh.primitives[j];

			size_t submeshFirstIndex = loaderInfo.indexPos;
			size_t submeshVertexStart = loaderInfo.vertexPos;
			size_t indexCount = 0;
			size_t vertexCount = 0;
			Vector3f posMin{};
			Vector3f posMax{};
			
			bool hasIndices = primitive.indices > -1;

			//vertices
			{
				const float* bufferPos = nullptr;
				const float* bufferNormals = nullptr;
				const float* bufferTangents = nullptr;
				const float* bufferTexCoordSet0 = nullptr;
				const float* bufferTexCoordSet1 = nullptr;
				const float* bufferColorSet0 = nullptr;
				const void* bufferJoints = nullptr;
				const float* bufferWeights = nullptr;

				int posByteStride;
				int normByteStride;
				int tangentByteStride;
				int uv0ByteStride;
				int uv1ByteStride;
				int color0ByteStride;
				int jointByteStride;
				int weightByteStride;

				int jointComponentType;

				ASSERT(primitive.attributes.find("POSITION") != primitive.attributes.end(), "Position attribute is required");

				const tinygltf::Accessor& posAccessor = model.accessors[primitive.attributes.find("POSITION")->second];
				const tinygltf::BufferView& posView = model.bufferViews[posAccessor.bufferView];
				bufferPos = reinterpret_cast<const float*>(&(model.buffers[posView.buffer].data[posAccessor.byteOffset + posView.byteOffset]));
				posMin = Vector3f((float)posAccessor.minValues[0], (float)posAccessor.minValues[1], (float)posAccessor.minValues[2]);
				posMax = posMin;
				vertexCount = static_cast<uint32_t>(posAccessor.count);
				posByteStride = posAccessor.ByteStride(posView) ? (posAccessor.ByteStride(posView) / sizeof(float)) : tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC3);

				if (primitive.attributes.find("NORMAL") != primitive.attributes.end()) {
					const tinygltf::Accessor& normAccessor = model.accessors[primitive.attributes.find("NORMAL")->second];
					const tinygltf::BufferView& normView = model.bufferViews[normAccessor.bufferView];
					bufferNormals = reinterpret_cast<const float*>(&(model.buffers[normView.buffer].data[normAccessor.byteOffset + normView.byteOffset]));
					normByteStride = normAccessor.ByteStride(normView) ? (normAccessor.ByteStride(normView) / sizeof(float)) : tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC3);
				}

				if (primitive.attributes.find("TANGENT") != primitive.attributes.end()) {
					const tinygltf::Accessor& tangentAccessor = model.accessors[primitive.attributes.find("TANGENT")->second];
					const tinygltf::BufferView& tangentView = model.bufferViews[tangentAccessor.bufferView];
					bufferTangents = reinterpret_cast<const float*>(&(model.buffers[tangentView.buffer].data[tangentAccessor.byteOffset + tangentView.byteOffset]));
					tangentByteStride = tangentAccessor.ByteStride(tangentView) ? (tangentAccessor.ByteStride(tangentView) / sizeof(float)) : tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC3);
				}

				// UVs
				if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end()) {
					const tinygltf::Accessor& uvAccessor = model.accessors[primitive.attributes.find("TEXCOORD_0")->second];
					const tinygltf::BufferView& uvView = model.bufferViews[uvAccessor.bufferView];
					bufferTexCoordSet0 = reinterpret_cast<const float*>(&(model.buffers[uvView.buffer].data[uvAccessor.byteOffset + uvView.byteOffset]));
					uv0ByteStride = uvAccessor.ByteStride(uvView) ? (uvAccessor.ByteStride(uvView) / sizeof(float)) : tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC2);
				}
				if (primitive.attributes.find("TEXCOORD_1") != primitive.attributes.end()) {
					const tinygltf::Accessor& uvAccessor = model.accessors[primitive.attributes.find("TEXCOORD_1")->second];
					const tinygltf::BufferView& uvView = model.bufferViews[uvAccessor.bufferView];
					bufferTexCoordSet1 = reinterpret_cast<const float*>(&(model.buffers[uvView.buffer].data[uvAccessor.byteOffset + uvView.byteOffset]));
					uv1ByteStride = uvAccessor.ByteStride(uvView) ? (uvAccessor.ByteStride(uvView) / sizeof(float)) : tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC2);
				}

				hasSkin |= (bufferJoints && bufferWeights);

				for (size_t v = 0; v < posAccessor.count; v++)
				{
					if (!hasSkin)
					{
						Vertex& vert = loaderInfo.vertexBuffer[loaderInfo.vertexPos];
						vert.position = Vector3f(bufferPos[v * posByteStride], bufferPos[v * posByteStride + 1], bufferPos[v * posByteStride + 2]);
						vert.normal = bufferNormals ? Vector3f(bufferNormals[v * normByteStride], bufferNormals[v * normByteStride + 1], bufferNormals[v * normByteStride + 2]) : Vector3f();
						//vert.tangent = bufferTangents ? Vector3f(bufferTangents[v * tangentByteStride], bufferTangents[v * tangentByteStride + 1], bufferTangents[v * tangentByteStride + 2]) : Vector3f();
						vert.tangent = Vector3f(1,1,1);
						vert.texcoord = bufferTexCoordSet0 ? Vector2f(bufferTexCoordSet0[v * uv0ByteStride], 1.0f - bufferTexCoordSet0[v * uv0ByteStride + 1]) : Vector2f();
					}

					loaderInfo.vertexPos++;
				}
			}

			//Indices
			{
				if (hasIndices)
				{
					const tinygltf::Accessor& accessor = model.accessors[primitive.indices > -1 ? primitive.indices : 0];
					const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
					const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

					indexCount = static_cast<size_t>(accessor.count);
					const void* dataPtr = &(buffer.data[accessor.byteOffset + bufferView.byteOffset]);

					switch (accessor.componentType)
					{
					case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT:
					{
						const uint32_t* buf = static_cast<const uint32_t*>(dataPtr);
						for (size_t index = 0; index < accessor.count; index++)
						{
							loaderInfo.indexBuffer[loaderInfo.indexPos] = buf[index] + vertexStart;
							loaderInfo.indexPos++;
						}
						break;
					}
					case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT: {
						const uint16_t* buf = static_cast<const uint16_t*>(dataPtr);
						for (size_t index = 0; index < accessor.count; index++) {
							loaderInfo.indexBuffer[loaderInfo.indexPos] = buf[index] + vertexStart;
							loaderInfo.indexPos++;
						}
						break;
					}
					case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE: {
						const uint8_t* buf = static_cast<const uint8_t*>(dataPtr);
						for (size_t index = 0; index < accessor.count; index++) {
							loaderInfo.indexBuffer[loaderInfo.indexPos] = buf[index] + vertexStart;
							loaderInfo.indexPos++;
						}
						break;
					}
					default:
						CLIENT_ERROR("Index component tpye {0} not supported!", accessor.componentType);
						break;
					}
				}
				
			}

			Submesh submesh;
			submesh.firstIndex = static_cast<uint32_t>(submeshFirstIndex);
			submesh.indexCount = static_cast<uint32_t>(indexCount);
			submesh.vertexOffset = static_cast<uint32_t>(submeshVertexStart);
			submesh.vertexCount = static_cast<uint32_t>(vertexCount);
			submesh.materialIndex = 0; // (primitive.material >= 0) ? primitive.material : 0; // TODO: load materials in first and the create a list of the ones used in this mesh and assign the correct index
			submesh.localTransform = Matrix4x4();
			submesh.transform = newNode->matrix;
			submesh.SetBoundingBox(posMin, posMax);

			submeshes.push_back(submesh);
		}

		//  TODO: Handle materials
		materials.resize(submeshes.size());
		for (size_t i = 0; i < materials.size(); i++)
		{
			if (!materials[i])
				materials[i] = Material::GetDefaultMaterial();
		}

		std::vector<Vertex> submeshVertices(loaderInfo.vertexBuffer.begin() + vertexStart, loaderInfo.vertexBuffer.begin() + loaderInfo.vertexPos);

		std::vector<float> finalVertexBuffer = GeometryGenerator::FlattenVertices(submeshVertices);

		std::vector<uint32_t> finalIndexBuffer(loaderInfo.indexBuffer.begin() + indexStart, loaderInfo.indexBuffer.begin() + loaderInfo.indexPos);

		Ref<Mesh> finalMesh = CreateRef<Mesh>(finalVertexBuffer, finalIndexBuffer, submeshes, materials, s_StaticMeshLayout);

		std::string meshName = mesh.name.empty() ? "Mesh_" + std::to_string(nodeIndex) : mesh.name;

		std::filesystem::path meshPath = (loaderInfo.destination / meshName).string() + ".staticmesh";

		MeshSerializer::Serialize(meshPath, finalMesh, hasSkin ? MeshLayout::SkinnedMesh : MeshLayout::StaticMesh);
	}
}

void glTFImporter::ImportAssets(const std::filesystem::path& filepath, const std::filesystem::path& destination)
{
	PROFILE_FUNCTION();

	LoaderInfo loaderInfo;
	loaderInfo.destination = destination;

	float globalScale = 1.0f;

	tinygltf::Model gltfModel;
	tinygltf::TinyGLTF gltfContext;

	std::string error;
	std::string warning;
	bool fileLoaded = false;

	if (filepath.extension() == ".glb")
		fileLoaded = gltfContext.LoadBinaryFromFile(&gltfModel, &error, &warning, filepath.string());
	else
		fileLoaded = gltfContext.LoadASCIIFromFile(&gltfModel, &error, &warning, filepath.string());

	if (!warning.empty())
		CLIENT_WARN("Problem loading gltf: {0}", warning);

	size_t vertexCount = 0;
	size_t indexCount = 0;

	if (fileLoaded)
	{
		const tinygltf::Scene& scene = gltfModel.scenes[gltfModel.defaultScene > -1 ? gltfModel.defaultScene : 0];

		for (const int& node : scene.nodes)
		{
			GetNodeProps(gltfModel.nodes[node], gltfModel, vertexCount, indexCount);
		}

		loaderInfo.vertexBuffer.resize(vertexCount);
		loaderInfo.indexBuffer.resize(indexCount);

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