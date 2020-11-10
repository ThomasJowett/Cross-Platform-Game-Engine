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

	Shape Type = Shape::Cube;

	bool NeedsUpdating = true;

	float CubeWidth = 1.0f;
	float CubeHeight = 1.0f;
	float CubeDepth = 1.0f;

	float SphereRadius = 0.5f;
	uint32_t SphereLongitudeLines = 16;
	uint32_t SphereLatitudeLines = 32;

	float PlaneWidth = 1.0f;
	float PlaneLength = 1.0f;
	uint32_t PlaneWidthLines = 2;
	uint32_t PlaneLengthLines = 2;
	float PlaneTileU = 1.0f;
	float PlaneTileV = 1.0f;

	float CylinderBottomRadius = 0.5f;
	float CylinderTopRadius = 0.5f;
	float CylinderHeight = 1.0f;
	uint32_t CylinderSliceCount = 32;
	uint32_t CylinderStackCount = 5;

	float ConeBottomRadius = 0.5f;
	float ConeHeight = 1.0f;
	uint32_t ConeSliceCount = 32;
	uint32_t ConeStackCount = 5;

	float TorusOuterRadius = 1.0f;
	float TorusInnerRadius = 0.4f;
	uint32_t TorusSliceCount = 32;

	PrimitiveComponent() = default;
	PrimitiveComponent(Shape shape)
		:Type(shape) {}
	PrimitiveComponent(const PrimitiveComponent&) = default;

	operator PrimitiveComponent::Shape& () { return Type;}
	operator const PrimitiveComponent::Shape& () { return Type; }

	template<typename Archive>
	void serialize(Archive& archive)
	{
		archive(cereal::make_nvp("Primitive Shape", Type));

		archive(cereal::make_nvp("Cube Width", CubeWidth));
		archive(cereal::make_nvp("Cube Height", CubeHeight));
		archive(cereal::make_nvp("Cube Depth", CubeDepth));

		archive(cereal::make_nvp("Sphere Radius", SphereRadius));
		archive(cereal::make_nvp("Sphere Longitude Lines", SphereLongitudeLines));
		archive(cereal::make_nvp("Sphere Latitude Lines", SphereLatitudeLines));


		archive(cereal::make_nvp("Plane Width", PlaneWidth));
		archive(cereal::make_nvp("Plane Length", PlaneLength));
		archive(cereal::make_nvp("Plane Width Lines", PlaneWidthLines));
		archive(cereal::make_nvp("Plane Length Lines", PlaneLengthLines));
		archive(cereal::make_nvp("Plane Tile U", PlaneTileU));
		archive(cereal::make_nvp("Plane Tile V", PlaneTileV));

		archive(cereal::make_nvp("Cylinder Bottom Radius", CylinderBottomRadius));
		archive(cereal::make_nvp("Cylinder Top Radius", CylinderTopRadius));
		archive(cereal::make_nvp("Cylinder Height", CylinderHeight));
		archive(cereal::make_nvp("Cylinder Slice Count", CylinderSliceCount));
		archive(cereal::make_nvp("Cylinder Stack Count", CylinderStackCount));

		archive(cereal::make_nvp("Cone Bottom Radius", ConeBottomRadius));
		archive(cereal::make_nvp("Cone Hiehgt", ConeHeight));
		archive(cereal::make_nvp("Cone Slice Count", ConeSliceCount));
		archive(cereal::make_nvp("Cone Stack Count", ConeStackCount));

		archive(cereal::make_nvp("Torus Outer Radius", TorusOuterRadius));
		archive(cereal::make_nvp("Torus Inner Radius", TorusInnerRadius));
		archive(cereal::make_nvp("Torus Slice Count", TorusSliceCount));
	}
};