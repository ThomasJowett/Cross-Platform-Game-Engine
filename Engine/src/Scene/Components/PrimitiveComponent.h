#pragma once

struct PrimitiveComponent
{
	Ref<Mesh> mesh;
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
		:type(shape) {}

	// Cube
	PrimitiveComponent(float cubeWidth, float cubeHeight, float cubeDepth)
		:type(Shape::Cube),
		cubeWidth(cubeWidth),
		cubeHeight(cubeHeight),
		cubeDepth(cubeDepth)
	{}

	//Sphere
	PrimitiveComponent(float shpereRadius, uint32_t sphereLongitudeLines, uint32_t sphereLatitudeLines)
		:type(Shape::Sphere),
		sphereRadius(shpereRadius),
		sphereLongitudeLines(sphereLongitudeLines),
		sphereLatitudeLines(sphereLatitudeLines)
	{}

	// Plane
	PrimitiveComponent(float planeWidth, float planeLength, uint32_t planeWidthLines, uint32_t planeLengthLines, float planeTileU, float planeTileV)
		:type(Shape::Plane),
		planeWidth(planeWidth),
		planeLength(planeLength),
		planeWidthLines(planeWidthLines),
		planeLengthLines(planeLengthLines),
		planeTileU(planeTileU),
		planeTileV(planeTileV)
	{}

	// Cylinder
	PrimitiveComponent(float cylinderBottomRadius, float cylinderTopRadius, float cylinderHeight, uint32_t cylinderSliceCount, uint32_t cylinderStackCount)
		:type(Shape::Cylinder),
		cylinderBottomRadius(cylinderBottomRadius),
		cylinderTopRadius(cylinderTopRadius),
		cylinderHeight(cylinderHeight),
		cylinderSliceCount(cylinderSliceCount),
		cylinderStackCount(cylinderStackCount)
	{}

	//Cone
	PrimitiveComponent(float coneBottomRadius, float coneHeight, uint32_t coneSliceCount, uint32_t coneStackCount)
		:type(Shape::Cone),
		coneBottomRadius(coneBottomRadius),
		coneHeight(coneHeight),
		coneSliceCount(coneSliceCount),
		coneStackCount(coneStackCount)
	{}

	//Torus
	PrimitiveComponent(float torusOuterRadius, float torusInnerRadius, uint32_t torusSliceCount)
		:type(Shape::Torus),
		torusOuterRadius(torusOuterRadius),
		torusInnerRadius(torusInnerRadius),
		torusSliceCount(torusSliceCount)
	{}

	PrimitiveComponent(const PrimitiveComponent&) = default;

	void SetCube(float width, float depth, float height)
	{
		cubeWidth = width;
		cubeDepth = depth;
		cubeHeight = height;
		type = Shape::Cube;
		needsUpdating = true;
	}

	void SetSphere(float radius, uint32_t longitudeLines, uint32_t latitudeLines)
	{
		sphereRadius = radius;
		sphereLongitudeLines = longitudeLines;
		sphereLatitudeLines = latitudeLines;
		type = Shape::Sphere;
		needsUpdating = true;
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
		needsUpdating = true;
	}

	void SetCylinder(float bottomRadius, float topRadius, float height, uint32_t sliceCount, uint32_t stackCount)
	{
		cylinderBottomRadius = bottomRadius;
		cylinderTopRadius = topRadius;
		cylinderHeight = height;
		cylinderSliceCount = sliceCount;
		cylinderStackCount = stackCount;
		type = Shape::Cylinder;
		needsUpdating = true;
	}

	void SetCone(float bottomRadius, float height, uint32_t sliceCount, uint32_t stackCount)
	{
		coneBottomRadius = bottomRadius;
		coneHeight = height;
		coneSliceCount = sliceCount;
		coneStackCount = stackCount;
		type = Shape::Cone;
		needsUpdating = true;
	}

	void SetTorus(float outerRadius, float innerRadius, uint32_t sliceCount)
	{
		torusOuterRadius = outerRadius;
		torusInnerRadius = innerRadius;
		torusSliceCount = sliceCount;
		type = Shape::Torus;
		needsUpdating = true;
	}

	operator PrimitiveComponent::Shape& () { return type; }
	operator const PrimitiveComponent::Shape& () { return type; }
private:
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
	}
};