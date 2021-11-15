#pragma once

struct PrimitiveComponent
{
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

	operator PrimitiveComponent::Shape& () { return type; }
	operator const PrimitiveComponent::Shape& () { return type; }

	template<typename Archive>
	void serialize(Archive& archive)
	{
		archive(cereal::make_nvp("Primitive Shape", type));

		archive(cereal::make_nvp("Cube Width", cubeWidth));
		archive(cereal::make_nvp("Cube Height", cubeHeight));
		archive(cereal::make_nvp("Cube Depth", cubeDepth));

		archive(cereal::make_nvp("Sphere Radius", sphereRadius));
		archive(cereal::make_nvp("Sphere Longitude Lines", sphereLongitudeLines));
		archive(cereal::make_nvp("Sphere Latitude Lines", sphereLatitudeLines));


		archive(cereal::make_nvp("Plane Width", planeWidth));
		archive(cereal::make_nvp("Plane Length", planeLength));
		archive(cereal::make_nvp("Plane Width Lines", planeWidthLines));
		archive(cereal::make_nvp("Plane Length Lines", planeLengthLines));
		archive(cereal::make_nvp("Plane Tile U", planeTileU));
		archive(cereal::make_nvp("Plane Tile V", planeTileV));

		archive(cereal::make_nvp("Cylinder Bottom Radius", cylinderBottomRadius));
		archive(cereal::make_nvp("Cylinder Top Radius", cylinderTopRadius));
		archive(cereal::make_nvp("Cylinder Height", cylinderHeight));
		archive(cereal::make_nvp("Cylinder Slice Count", cylinderSliceCount));
		archive(cereal::make_nvp("Cylinder Stack Count", cylinderStackCount));

		archive(cereal::make_nvp("Cone Bottom Radius", coneBottomRadius));
		archive(cereal::make_nvp("Cone Height", coneHeight));
		archive(cereal::make_nvp("Cone Slice Count", coneSliceCount));
		archive(cereal::make_nvp("Cone Stack Count", coneStackCount));

		archive(cereal::make_nvp("Torus Outer Radius", torusOuterRadius));
		archive(cereal::make_nvp("Torus Inner Radius", torusInnerRadius));
		archive(cereal::make_nvp("Torus Slice Count", torusSliceCount));
	}
};