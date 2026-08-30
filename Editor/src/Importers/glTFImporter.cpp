#include "glTFImporter.h"

#include "Utilities/MeshSerializer.h"
#include "Utilities/GeometryGenerator.h"
#include "Utilities/FileUtils.h"
#include "math/Matrix.h"
#include "math/Quaternion.h"
#include "Core/Colour.h"
#include "Core/Application.h"
#include "Asset/Material.h"
#include "Scene/AssetManager.h"
#include "Logging/Logger.h"

#include <cfloat>

#define TINYGLTF_IMPLEMENTATION
// STB_IMAGE and STB_IMAGE_WRITE implementations are already in Engine/vendor/stb/stb.cpp
#include "tiny_gltf.h"

namespace gltf_importer_internal {

struct GltfLoaderInfo {
	std::vector<uint32_t> indexBuffer;
	std::vector<Vertex> vertexBuffer;
	size_t indexPos = 0;
	size_t vertexPos = 0;
	std::filesystem::path destination;
	std::filesystem::path assetDir;
};

struct GltfImportTexture
{
	int index = -1;
	std::filesystem::path path;
	bool is_valid = false;
};

struct GltfImportMaterial
{
	int index = -1;
	std::string name;
	GltfImportTexture textures[3]; // 0: Albedo, 1: Normal, 2: MetallicRoughness
	Colour baseColorFactor{ 1.0f, 1.0f, 1.0f, 1.0f };
};

void LoadNode(const Matrix4x4& parentTransform, const tinygltf::Node& node, uint32_t nodeIndex, const tinygltf::Model& model, GltfLoaderInfo& loaderInfo, const std::vector<GltfImportMaterial>& importMaterials)
{
	PROFILE_FUNCTION();

	Matrix4x4 localTransform;
	if (node.matrix.size() == 16)
	{
		localTransform = Matrix4x4(
			(float)node.matrix[0], (float)node.matrix[4], (float)node.matrix[8], (float)node.matrix[12],
			(float)node.matrix[1], (float)node.matrix[5], (float)node.matrix[9], (float)node.matrix[13],
			(float)node.matrix[2], (float)node.matrix[6], (float)node.matrix[10], (float)node.matrix[14],
			(float)node.matrix[3], (float)node.matrix[7], (float)node.matrix[11], (float)node.matrix[15]
		);
	}
	else
	{
		Vector3f translation(0.0f, 0.0f, 0.0f);
		Quaternion rotation;
		Vector3f scale(1.0f, 1.0f, 1.0f);

		if (node.translation.size() == 3) translation = Vector3f((float)node.translation[0], (float)node.translation[1], (float)node.translation[2]);
		// glTF stores quaternions as [x, y, z, w]; Quaternion's constructor takes (r, i, j, k) i.e. (w, x, y, z).
		if (node.rotation.size() == 4) rotation = Quaternion((float)node.rotation[3], (float)node.rotation[0], (float)node.rotation[1], (float)node.rotation[2]);
		if (node.scale.size() == 3) scale = Vector3f((float)node.scale[0], (float)node.scale[1], (float)node.scale[2]);

		localTransform = Matrix4x4::Translate(translation) * Matrix4x4::Rotate(rotation) * Matrix4x4::Scale(scale);
	}

	Matrix4x4 worldTransform = parentTransform * localTransform;
	Matrix4x4 rotationMatrix = Matrix4x4::Rotate(worldTransform.ExtractRotation());

	if (node.mesh > -1)
	{
		const tinygltf::Mesh mesh = model.meshes[node.mesh];

		std::vector<Submesh> submeshes;
		std::vector<Ref<Material>> engineMaterials;

		size_t meshVertexStart = loaderInfo.vertexPos;
		size_t meshIndexStart = loaderInfo.indexPos;

		for (size_t j = 0; j < mesh.primitives.size(); j++)
		{
			const tinygltf::Primitive& primitive = mesh.primitives[j];

			size_t submeshFirstIndex = loaderInfo.indexPos;
			size_t submeshVertexOffset = loaderInfo.vertexPos;
			
			const tinygltf::Accessor& posAccessor = model.accessors[primitive.attributes.find("POSITION")->second];
			const tinygltf::BufferView& posView = model.bufferViews[posAccessor.bufferView];
			const float* bufferPos = reinterpret_cast<const float*>(&(model.buffers[posView.buffer].data[posAccessor.byteOffset + posView.byteOffset]));
			int posByteStride = posAccessor.ByteStride(posView) / sizeof(float);
			uint32_t vertexCount = static_cast<uint32_t>(posAccessor.count);

			const float* bufferNormals = nullptr;
			int normByteStride = 0;
			if (primitive.attributes.find("NORMAL") != primitive.attributes.end()) {
				const tinygltf::Accessor& normAccessor = model.accessors[primitive.attributes.find("NORMAL")->second];
				const tinygltf::BufferView& normView = model.bufferViews[normAccessor.bufferView];
				bufferNormals = reinterpret_cast<const float*>(&(model.buffers[normView.buffer].data[normAccessor.byteOffset + normView.byteOffset]));
				normByteStride = normAccessor.ByteStride(normView) / sizeof(float);
			}

			const float* bufferTangents = nullptr;
			int tangentByteStride = 0;
			if (primitive.attributes.find("TANGENT") != primitive.attributes.end()) {
				const tinygltf::Accessor& tangentAccessor = model.accessors[primitive.attributes.find("TANGENT")->second];
				const tinygltf::BufferView& tangentView = model.bufferViews[tangentAccessor.bufferView];
				bufferTangents = reinterpret_cast<const float*>(&(model.buffers[tangentView.buffer].data[tangentAccessor.byteOffset + tangentView.byteOffset]));
				tangentByteStride = tangentAccessor.ByteStride(tangentView) / sizeof(float);
			}

			const float* bufferUVs = nullptr;
			int uvByteStride = 0;
			if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end()) {
				const tinygltf::Accessor& uvAccessor = model.accessors[primitive.attributes.find("TEXCOORD_0")->second];
				const tinygltf::BufferView& uvView = model.bufferViews[uvAccessor.bufferView];
				bufferUVs = reinterpret_cast<const float*>(&(model.buffers[uvView.buffer].data[uvAccessor.byteOffset + uvView.byteOffset]));
				uvByteStride = uvAccessor.ByteStride(uvView) / sizeof(float);
			}

			Vector3f subMin(FLT_MAX, FLT_MAX, FLT_MAX), subMax(-FLT_MAX, -FLT_MAX, -FLT_MAX);

			for (size_t v = 0; v < vertexCount; v++)
			{
				Vertex vert;
				vert.position = worldTransform * Vector3f(bufferPos[v * posByteStride], bufferPos[v * posByteStride + 1], bufferPos[v * posByteStride + 2]);
				
				subMin.x = std::min(subMin.x, vert.position.x); subMin.y = std::min(subMin.y, vert.position.y); subMin.z = std::min(subMin.z, vert.position.z);
				subMax.x = std::max(subMax.x, vert.position.x); subMax.y = std::max(subMax.y, vert.position.y); subMax.z = std::max(subMax.z, vert.position.z);

				if (bufferNormals) vert.normal = (rotationMatrix * Vector3f(bufferNormals[v * normByteStride], bufferNormals[v * normByteStride + 1], bufferNormals[v * normByteStride + 2])).GetNormalized();
				if (bufferTangents) vert.tangent = (rotationMatrix * Vector3f(bufferTangents[v * tangentByteStride], bufferTangents[v * tangentByteStride + 1], bufferTangents[v * tangentByteStride + 2])).GetNormalized();
				// glTF UVs have (0,0) at the top-left of the image; this engine's textures are loaded
				// with stbi_set_flip_vertically_on_load(1), i.e. (0,0) at the bottom-left - flip V to match.
				if (bufferUVs) vert.texcoord = Vector2f(bufferUVs[v * uvByteStride], 1.0f - bufferUVs[v * uvByteStride + 1]);

				loaderInfo.vertexBuffer.push_back(vert);
				loaderInfo.vertexPos++;
			}

			size_t indexCount = 0;
			if (primitive.indices > -1)
			{
				const tinygltf::Accessor& accessor = model.accessors[primitive.indices];
				const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
				const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];
				const void* dataPtr = &(buffer.data[accessor.byteOffset + bufferView.byteOffset]);
				indexCount = accessor.count;

				for (size_t i = 0; i < indexCount; i++)
				{
					uint32_t idx = 0;
					if (accessor.componentType == TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT) idx = static_cast<const uint32_t*>(dataPtr)[i];
					else if (accessor.componentType == TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT) idx = static_cast<const uint16_t*>(dataPtr)[i];
					else if (accessor.componentType == TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE) idx = static_cast<const uint8_t*>(dataPtr)[i];
					// Local to this primitive's own vertex block - submesh.vertexOffset below supplies
					// the offset once, at draw time. Baking it in here too double-offset everything
					// past the first submesh; and since the mesh's own vertex/index arrays get sliced
					// out relative to meshVertexStart/meshIndexStart further down, a scene-wide-absolute
					// value here would be wrong for every mesh after the first in the scene anyway.
					loaderInfo.indexBuffer.push_back(idx);
					loaderInfo.indexPos++;
				}
			}

			Submesh submesh;
			submesh.firstIndex = static_cast<uint32_t>(submeshFirstIndex - meshIndexStart);
			submesh.indexCount = static_cast<uint32_t>(indexCount);
			submesh.vertexOffset = static_cast<uint32_t>(submeshVertexOffset - meshVertexStart);
			submesh.vertexCount = static_cast<uint32_t>(vertexCount);
			// Vertex positions above are already baked into world space via worldTransform, and
			// Renderer::Submit combines submesh.transform with the entity's own transform at draw
			// time - storing worldTransform here too would apply it a second time.
			submesh.transform = Matrix4x4();
			submesh.localTransform = Matrix4x4();
			submesh.SetBoundingBox(subMin, subMax);

			if (primitive.material >= 0 && primitive.material < (int)importMaterials.size())
			{
				const GltfImportMaterial& imat = importMaterials[primitive.material];
				std::filesystem::path matPath = loaderInfo.destination / (imat.name + ".material");
				// Relative to the project - matches how every other asset reference is stored, and is
				// what ends up in this submesh's serialized material name (Mesh::GetMaterials(),
				// MeshSerializer::Serialize). Using the absolute matPath directly would still "work"
				// via Material::Load's own absolute() fallback, but bakes this machine's exact folder
				// layout into the saved .staticmesh instead of a portable, relative reference.
				std::filesystem::path matRelPath = FileUtils::RelativePath(matPath, Application::GetOpenDocumentDirectory());
				Ref<Material> engineMat;
				if (!std::filesystem::exists(matPath))
				{
					Ref<Material> newMat = CreateRef<Material>();
					newMat->SetShader("Standard");
					newMat->SetTint(imat.baseColorFactor);
					// imat.textures[*].path is absolute (needed for the copy_file call in
					// ProcessTexture, which isn't relative-to-project aware) - but Texture2D::Create
					// stores whatever path it's given as the texture's own filepath, and that ends up
					// serialized into this .material file, so pass it a project-relative path instead
					// or every texture reference bakes in this machine's exact folder layout.
					auto toProjectRelative = [](const std::filesystem::path& absolutePath)
					{
						return FileUtils::RelativePath(absolutePath, Application::GetOpenDocumentDirectory());
					};
					if (imat.textures[0].is_valid) newMat->AddTexture(Texture2D::Create(toProjectRelative(imat.textures[0].path)), 0);
					if (imat.textures[1].is_valid) newMat->AddTexture(Texture2D::Create(toProjectRelative(imat.textures[1].path)), 1);
					if (imat.textures[2].is_valid) newMat->AddTexture(Texture2D::Create(toProjectRelative(imat.textures[2].path)), 2);
					newMat->SaveMaterial(matRelPath);
				}
				// SaveMaterial() is const and never sets the saved material's own filepath - reload
				// through AssetManager (also lets primitives sharing one material reuse the same
				// cached instance) so this submesh's material actually has a filepath to serialize,
				// rather than silently collapsing to the default material on the next load.
				engineMat = AssetManager::GetAsset<Material>(matRelPath);
				submesh.materialIndex = (uint32_t)engineMaterials.size();
				engineMaterials.push_back(engineMat);
			}
			else
			{
				submesh.materialIndex = (uint32_t)engineMaterials.size();
				engineMaterials.push_back(Material::GetDefaultMaterial());
			}

			submeshes.push_back(submesh);

			if (!bufferTangents && bufferUVs)
			{
				GeometryGenerator::GenerateTangents(&loaderInfo.vertexBuffer[submeshVertexOffset], vertexCount);
			}
		}

		std::vector<Vertex> meshVertices(loaderInfo.vertexBuffer.begin() + meshVertexStart, loaderInfo.vertexBuffer.begin() + loaderInfo.vertexPos);
		std::vector<uint32_t> meshIndices(loaderInfo.indexBuffer.begin() + meshIndexStart, loaderInfo.indexBuffer.begin() + loaderInfo.indexPos);

		Ref<Mesh> finalMesh = CreateRef<Mesh>(GeometryGenerator::FlattenVertices(meshVertices), meshIndices, submeshes, engineMaterials, s_StaticMeshLayout);
		std::string meshName = mesh.name.empty() ? "Mesh_" + std::to_string(nodeIndex) : mesh.name;
		std::replace_if(meshName.begin(), meshName.end(), [](char c) {return !std::isalnum(c); }, '_');
		std::filesystem::path meshPath = loaderInfo.destination / (meshName + ".staticmesh");
		MeshSerializer::Serialize(meshPath, finalMesh, MeshLayout::StaticMesh);
	}

	for (int childIndex : node.children)
	{
		LoadNode(worldTransform, model.nodes[childIndex], childIndex, model, loaderInfo, importMaterials);
	}
}

} // namespace gltf_importer_internal

using namespace gltf_importer_internal;

void glTFImporter::ImportAssets(const std::filesystem::path& filepath, const std::filesystem::path& destination)
{
	PROFILE_FUNCTION();

	GltfLoaderInfo loaderInfo;
	loaderInfo.destination = destination;
	loaderInfo.assetDir = filepath.parent_path();

	tinygltf::Model gltfModel;
	tinygltf::TinyGLTF gltfContext;
	std::string error, warning;
	bool fileLoaded = (filepath.extension() == ".glb") ? gltfContext.LoadBinaryFromFile(&gltfModel, &error, &warning, filepath.string()) : gltfContext.LoadASCIIFromFile(&gltfModel, &error, &warning, filepath.string());

	if (!warning.empty()) CLIENT_WARN("glTF Warning: {0}", warning);
	if (!fileLoaded) { CLIENT_ERROR("glTF Error: {0}", error); return; }

	// Process Materials and Textures
	std::vector<GltfImportMaterial> importMaterials;
	for (size_t i = 0; i < gltfModel.materials.size(); i++)
	{
		const tinygltf::Material& gltfMat = gltfModel.materials[i];
		GltfImportMaterial imat;
		imat.name = gltfMat.name.empty() ? "Material_" + std::to_string(i) : gltfMat.name;
		std::replace_if(imat.name.begin(), imat.name.end(), [](char c) {return !std::isalnum(c); }, '_');

		auto& pbr = gltfMat.pbrMetallicRoughness;
		imat.baseColorFactor = Colour((float)pbr.baseColorFactor[0], (float)pbr.baseColorFactor[1], (float)pbr.baseColorFactor[2], (float)pbr.baseColorFactor[3]);

		auto ProcessTexture = [&](int textureIndex, int slot) {
			if (textureIndex >= 0)
			{
				const tinygltf::Texture& tex = gltfModel.textures[textureIndex];
				if (tex.source >= 0)
				{
					const tinygltf::Image& img = gltfModel.images[tex.source];
					std::filesystem::path srcPath;
					if (!img.uri.empty())
					{
						srcPath = loaderInfo.assetDir / img.uri;
					}
					
					if (std::filesystem::exists(srcPath))
					{
						imat.textures[slot].path = destination / srcPath.filename();
						if (!std::filesystem::exists(imat.textures[slot].path))
							std::filesystem::copy_file(srcPath, imat.textures[slot].path);
						imat.textures[slot].is_valid = true;
					}
				}
			}
		};

		ProcessTexture(pbr.baseColorTexture.index, 0);
		ProcessTexture(gltfMat.normalTexture.index, 1);
		ProcessTexture(pbr.metallicRoughnessTexture.index, 2);

		importMaterials.push_back(imat);
	}

	const tinygltf::Scene& scene = gltfModel.scenes[gltfModel.defaultScene > -1 ? gltfModel.defaultScene : 0];
	for (int nodeIndex : scene.nodes)
	{
		LoadNode(Matrix4x4(), gltfModel.nodes[nodeIndex], nodeIndex, gltfModel, loaderInfo, importMaterials);
	}
}
