#pragma once
#include "Utilities/GeometryGenerator.h"
#include "Core/BoundingBox.h"
#include "Scene/AssetManager.h"

struct PrimitiveComponent
{
	Ref<VertexArray> mesh;
	Ref<Material> material;

	enum class Shape
	{
		Cube,
		Sphere,
		Plane,
		Cylinder,
		Cone,
		Torus
	};

	Shape type = Shape::Cube;

	bool needsUpdating = true;

	float cubeWidth = 1.0f;
	float cubeHeight = 1.0f;
	float cubeDepth = 1.0f;

	float sphereRadius = 0.5f;
	uint32_t sphereLongitudeLines = 16;
	uint32_t sphereLatitudeLines = 32;

	float planeWidth = 1.0f;
	float planeLength = 1.0f;
	uint32_t planeWidthLines = 2;
	uint32_t planeLengthLines = 2;
	float planeTileU = 1.0f;
	float planeTileV = 1.0f;

	float cylinderBottomRadius = 0.5f;
	float cylinderTopRadius = 0.5f;
	float cylinderHeight = 1.0f;
	uint32_t cylinderSliceCount = 32;
	uint32_t cylinderStackCount = 5;

	float coneBottomRadius = 0.5f;
	float coneHeight = 1.0f;
	uint32_t coneSliceCount = 32;
	uint32_t coneStackCount = 5;

	float torusOuterRadius = 1.0f;
	float torusInnerRadius = 0.4f;
	uint32_t torusSliceCount = 32;

	PrimitiveComponent() = default;
	PrimitiveComponent(Shape shape)
		:type(shape) {
		SetType(shape);
	}

	// Cube
	PrimitiveComponent(float cubeWidth, float cubeHeight, float cubeDepth)
	{
		SetCube(cubeWidth, cubeHeight, cubeDepth);
	}

	//Sphere
	PrimitiveComponent(float shpereRadius, uint32_t sphereLongitudeLines, uint32_t sphereLatitudeLines)
	{
		SetSphere(shpereRadius, sphereLongitudeLines, sphereLatitudeLines);
	}

	// Plane
	PrimitiveComponent(float planeWidth, float planeLength, uint32_t planeWidthLines, uint32_t planeLengthLines, float planeTileU, float planeTileV)
	{
		SetPlane(planeWidth, planeLength, planeWidthLines, planeLengthLines, planeTileU, planeTileV);
	}

	// Cylinder
	PrimitiveComponent(float cylinderBottomRadius, float cylinderTopRadius, float cylinderHeight, uint32_t cylinderSliceCount, uint32_t cylinderStackCount)
	{
		SetCylinder(cylinderBottomRadius, cylinderTopRadius, cylinderHeight, cylinderSliceCount, cylinderStackCount);
	}

	//Cone
	PrimitiveComponent(float coneBottomRadius, float coneHeight, uint32_t coneSliceCount, uint32_t coneStackCount)
	{
		SetCone(coneBottomRadius, coneHeight, coneSliceCount, coneStackCount);
	}

	//Torus
	PrimitiveComponent(float torusOuterRadius, float torusInnerRadius, uint32_t torusSliceCount)
	{
		SetTorus(torusOuterRadius, torusInnerRadius, torusSliceCount);
	}

	PrimitiveComponent(const PrimitiveComponent&) = default;

	void SetCube(float width, float depth, float height)
	{
		cubeWidth = width;
		cubeDepth = depth;
		cubeHeight = height;
		type = Shape::Cube;
		needsUpdating = false;
		mesh = GeometryGenerator::CreateCube(cubeWidth, cubeHeight, cubeDepth);
		if (!material) material = AssetManager::GetMaterial("_");

		m_Bounds = BoundingBox(Vector3f(-width * .5f, -height * .5f, -depth * .5f), Vector3f(width * .5f, height * .5f, depth * .5f));
	}

	void SetSphere(float radius, uint32_t longitudeLines, uint32_t latitudeLines)
	{
		sphereRadius = radius;
		sphereLongitudeLines = longitudeLines;
		sphereLatitudeLines = latitudeLines;
		type = Shape::Sphere;
		needsUpdating = false;
		mesh = GeometryGenerator::CreateSphere(radius, longitudeLines, latitudeLines);
		if (!material) material = AssetManager::GetMaterial("_");

		m_Bounds = BoundingBox(Vector3f(-radius, -radius, -radius), Vector3f(radius, radius, radius));
	}

	void SetPlane(float width, float length, uint32_t widthLines, uint32_t lengthLines, float tileU, float tileV)
	{
		planeWidth = width;
		planeLength = length;
		planeWidthLines = widthLines;
		planeLengthLines = lengthLines;
		planeTileU = tileU;
		planeTileV = tileV;
		type = Shape::Plane;
		needsUpdating = false;
		mesh = GeometryGenerator::CreateGrid(width, length, widthLines, lengthLines, tileU, tileV);
		if (!material) material = AssetManager::GetMaterial("_");

		m_Bounds = BoundingBox(Vector3f(-width * .5f, -0.0f, -length * .5f), Vector3f(width * .5f, 0.0f, length * .5f));
	}

	void SetCylinder(float bottomRadius, float topRadius, float height, uint32_t sliceCount, uint32_t stackCount)
	{
		cylinderBottomRadius = bottomRadius;
		cylinderTopRadius = topRadius;
		cylinderHeight = height;
		cylinderSliceCount = sliceCount;
		cylinderStackCount = stackCount;
		type = Shape::Cylinder;
		needsUpdating = false;
		mesh = GeometryGenerator::CreateCylinder(bottomRadius, topRadius, height, sliceCount, stackCount);
		if (!material) material = AssetManager::GetMaterial("_");
		float maxRadius = std::max(bottomRadius, topRadius);
		m_Bounds = BoundingBox(Vector3f(-maxRadius, -height * 0.5f, -maxRadius), Vector3f(maxRadius, height * 0.5f, maxRadius));
	}

	void SetCone(float bottomRadius, float height, uint32_t sliceCount, uint32_t stackCount)
	{
		coneBottomRadius = bottomRadius;
		coneHeight = height;
		coneSliceCount = sliceCount;
		coneStackCount = stackCount;
		type = Shape::Cone;
		needsUpdating = false;
		mesh = GeometryGenerator::CreateCylinder(bottomRadius, 0, height, sliceCount, stackCount);
		if (!material) material = AssetManager::GetMaterial("_");
		m_Bounds = BoundingBox(Vector3f(-bottomRadius, -height * 0.5f, -bottomRadius), Vector3f(bottomRadius, height * 0.5f, bottomRadius));
	}

	void SetTorus(float outerRadius, float innerRadius, uint32_t sliceCount)
	{
		torusOuterRadius = outerRadius;
		torusInnerRadius = innerRadius;
		torusSliceCount = sliceCount;
		type = Shape::Torus;
		needsUpdating = false;
		mesh = GeometryGenerator::CreateTorus(outerRadius, innerRadius, sliceCount);
		if(!material) material = AssetManager::GetMaterial("_");
		m_Bounds = BoundingBox(Vector3f(-(outerRadius + innerRadius) * .5f, -innerRadius * .5f, -(outerRadius + innerRadius) * .5f), Vector3f((outerRadius + innerRadius) * .5f, innerRadius * .5f, (outerRadius + innerRadius) * .5f));
	}

	void SetType(Shape type)
	{
		switch (type)
		{
		case PrimitiveComponent::Shape::Cube:
			SetCube(cubeWidth, cubeDepth, cubeHeight);
			return;
		case PrimitiveComponent::Shape::Sphere:
			SetSphere(sphereRadius, sphereLongitudeLines, sphereLatitudeLines);
			return;
		case PrimitiveComponent::Shape::Plane:
			SetPlane(planeWidth, planeLength, planeWidthLines, planeLengthLines, planeTileU, planeTileV);
			return;
		case PrimitiveComponent::Shape::Cylinder:
			SetCylinder(cylinderBottomRadius, cylinderTopRadius, cylinderHeight, cylinderSliceCount, cylinderStackCount);
			return;
		case PrimitiveComponent::Shape::Cone:
			SetCone(coneBottomRadius, coneHeight, coneSliceCount, coneStackCount);
			return;
		case PrimitiveComponent::Shape::Torus:
			SetTorus(torusOuterRadius, torusInnerRadius, torusSliceCount);
			return;
		}
	}

	const BoundingBox& GetBounds() { return m_Bounds; }

	operator PrimitiveComponent::Shape& () { return type; }
	operator const PrimitiveComponent::Shape& () { return type; }
private:
	BoundingBox m_Bounds;
	friend cereal::access;

	template<typename Archive>
	void save(Archive& archive) const
	{
		archive(type,
			cubeWidth, cubeHeight, cubeDepth,
			sphereRadius, sphereLongitudeLines, sphereLatitudeLines,
			planeWidth, planeLength, planeWidthLines, planeLengthLines, planeTileU, planeTileV,
			cylinderBottomRadius, cylinderTopRadius, cylinderHeight, cylinderSliceCount, cylinderStackCount,
			coneBottomRadius, coneHeight, coneSliceCount, coneStackCount,
			torusOuterRadius, torusInnerRadius, torusSliceCount);

		std::string relativePath;
		if (material && !material->GetFilepath().empty())
		{
			relativePath = FileUtils::RelativePath(material->GetFilepath(), Application::GetOpenDocumentDirectory()).string();
		}
		archive(cereal::make_nvp("Material", relativePath));
	}

	template<typename Archive>
	void load(Archive& archive)
	{
		archive(type,
			cubeWidth, cubeHeight, cubeDepth,
			sphereRadius, sphereLongitudeLines, sphereLatitudeLines,
			planeWidth, planeLength, planeWidthLines, planeLengthLines, planeTileU, planeTileV,
			cylinderBottomRadius, cylinderTopRadius, cylinderHeight, cylinderSliceCount, cylinderStackCount,
			coneBottomRadius, coneHeight, coneSliceCount, coneStackCount,
			torusOuterRadius, torusInnerRadius, torusSliceCount);
		std::string relativePath;

		archive(cereal::make_nvp("Material", relativePath));
		if (!relativePath.empty())
		{
			material = AssetManager::GetMaterial(std::filesystem::absolute(Application::GetOpenDocumentDirectory() / relativePath));
		}
		SetType(type);
	}
};