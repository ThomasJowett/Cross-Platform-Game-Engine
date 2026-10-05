#pragma once
#include "Utilities/GeometryGenerator.h"
#include "Core/BoundingBox.h"
#include "Scene/AssetManager.h"
#include "Scripting/Lua/LuaBindings.h"

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
		:type(shape) {
		SetType(shape);
	}

	// Cube
	PrimitiveComponent(float cubeWidth, float cubeHeight, float cubeDepth)
	{
		SetCube(cubeWidth, cubeHeight, cubeDepth);
	}

	//Sphere
	PrimitiveComponent(float sphereRadius, uint32_t sphereLongitudeLines, uint32_t sphereLatitudeLines)
	{
		SetSphere(sphereRadius, sphereLongitudeLines, sphereLatitudeLines);
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
		if (!material) material = Material::GetDefaultMaterial();
	}

	void SetSphere(float radius, uint32_t longitudeLines, uint32_t latitudeLines)
	{
		sphereRadius = radius;
		sphereLongitudeLines = longitudeLines;
		sphereLatitudeLines = latitudeLines;
		type = Shape::Sphere;
		needsUpdating = false;
		mesh = GeometryGenerator::CreateSphere(radius, longitudeLines, latitudeLines);
		if (!material) material = Material::GetDefaultMaterial();
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
		if (!material) material = Material::GetDefaultMaterial();
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
		if (!material) material = Material::GetDefaultMaterial();
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
		if (!material) material = Material::GetDefaultMaterial();
	}

	void SetTorus(float outerRadius, float innerRadius, uint32_t sliceCount)
	{
		torusOuterRadius = outerRadius;
		torusInnerRadius = innerRadius;
		torusSliceCount = sliceCount;
		type = Shape::Torus;
		needsUpdating = false;
		mesh = GeometryGenerator::CreateTorus(outerRadius, innerRadius, sliceCount);
		if (!material) material = Material::GetDefaultMaterial();
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

	operator PrimitiveComponent::Shape& () { return type; }
	operator const PrimitiveComponent::Shape& () { return type; }
	REFLECT_LUA_BEGIN(PrimitiveComponent)
		REFLECT_LUA_PROPERTY_CUSTOM("Type", "Shape as an integer: 0 Cube, 1 Sphere, 2 Plane, 3 Cylinder, 4 Cone, 5 Torus", "integer (enum)",
			([](Self& c) { return c.type; }),
			([](Self& c, Shape v) { c.type = v; }))
		REFLECT_LUA_PROPERTY_CUSTOM("CubeWidth", "Shape setting - doesn't rebuild the mesh; call the matching Set function to apply it", "number",
			([](Self& c) { return c.cubeWidth; }),
			([](Self& c, float v) { c.cubeWidth = v; }))
		REFLECT_LUA_PROPERTY_CUSTOM("CubeHeight", "Shape setting - doesn't rebuild the mesh; call the matching Set function to apply it", "number",
			([](Self& c) { return c.cubeHeight; }),
			([](Self& c, float v) { c.cubeHeight = v; }))
		REFLECT_LUA_PROPERTY_CUSTOM("CubeDepth", "Shape setting - doesn't rebuild the mesh; call the matching Set function to apply it", "number",
			([](Self& c) { return c.cubeDepth; }),
			([](Self& c, float v) { c.cubeDepth = v; }))
		REFLECT_LUA_PROPERTY_CUSTOM("SphereRadius", "Shape setting - doesn't rebuild the mesh; call the matching Set function to apply it", "number",
			([](Self& c) { return c.sphereRadius; }),
			([](Self& c, float v) { c.sphereRadius = v; }))
		REFLECT_LUA_PROPERTY_CUSTOM("SphereLongitudeLines", "Shape setting - doesn't rebuild the mesh; call the matching Set function to apply it", "integer",
			([](Self& c) { return c.sphereLongitudeLines; }),
			([](Self& c, uint32_t v) { c.sphereLongitudeLines = v; }))
		REFLECT_LUA_PROPERTY_CUSTOM("SphereLatitudeLines", "Shape setting - doesn't rebuild the mesh; call the matching Set function to apply it", "integer",
			([](Self& c) { return c.sphereLatitudeLines; }),
			([](Self& c, uint32_t v) { c.sphereLatitudeLines = v; }))
		REFLECT_LUA_PROPERTY_CUSTOM("PlaneWidth", "Shape setting - doesn't rebuild the mesh; call the matching Set function to apply it", "number",
			([](Self& c) { return c.planeWidth; }),
			([](Self& c, float v) { c.planeWidth = v; }))
		REFLECT_LUA_PROPERTY_CUSTOM("PlaneLength", "Shape setting - doesn't rebuild the mesh; call the matching Set function to apply it", "number",
			([](Self& c) { return c.planeLength; }),
			([](Self& c, float v) { c.planeLength = v; }))
		REFLECT_LUA_PROPERTY_CUSTOM("PlaneWidthLines", "Shape setting - doesn't rebuild the mesh; call the matching Set function to apply it", "integer",
			([](Self& c) { return c.planeWidthLines; }),
			([](Self& c, uint32_t v) { c.planeWidthLines = v; }))
		REFLECT_LUA_PROPERTY_CUSTOM("PlaneLengthLines", "Shape setting - doesn't rebuild the mesh; call the matching Set function to apply it", "integer",
			([](Self& c) { return c.planeLengthLines; }),
			([](Self& c, uint32_t v) { c.planeLengthLines = v; }))
		REFLECT_LUA_PROPERTY_CUSTOM("PlaneTileU", "Shape setting - doesn't rebuild the mesh; call the matching Set function to apply it", "number",
			([](Self& c) { return c.planeTileU; }),
			([](Self& c, float v) { c.planeTileU = v; }))
		REFLECT_LUA_PROPERTY_CUSTOM("PlaneTileV", "Shape setting - doesn't rebuild the mesh; call the matching Set function to apply it", "number",
			([](Self& c) { return c.planeTileV; }),
			([](Self& c, float v) { c.planeTileV = v; }))
		REFLECT_LUA_PROPERTY_CUSTOM("ConeBottomRadius", "Shape setting - doesn't rebuild the mesh; call the matching Set function to apply it", "number",
			([](Self& c) { return c.coneBottomRadius; }),
			([](Self& c, float v) { c.coneBottomRadius = v; }))
		REFLECT_LUA_PROPERTY_CUSTOM("ConeHeight", "Shape setting - doesn't rebuild the mesh; call the matching Set function to apply it", "number",
			([](Self& c) { return c.coneHeight; }),
			([](Self& c, float v) { c.coneHeight = v; }))
		REFLECT_LUA_PROPERTY_CUSTOM("ConeSliceCount", "Shape setting - doesn't rebuild the mesh; call the matching Set function to apply it", "integer",
			([](Self& c) { return c.coneSliceCount; }),
			([](Self& c, uint32_t v) { c.coneSliceCount = v; }))
		REFLECT_LUA_PROPERTY_CUSTOM("ConeStackCount", "Shape setting - doesn't rebuild the mesh; call the matching Set function to apply it", "integer",
			([](Self& c) { return c.coneStackCount; }),
			([](Self& c, uint32_t v) { c.coneStackCount = v; }))
		REFLECT_LUA_PROPERTY_CUSTOM("CylinderBottomRadius", "Shape setting - doesn't rebuild the mesh; call the matching Set function to apply it", "number",
			([](Self& c) { return c.cylinderBottomRadius; }),
			([](Self& c, float v) { c.cylinderBottomRadius = v; }))
		REFLECT_LUA_PROPERTY_CUSTOM("CylinderTopRadius", "Shape setting - doesn't rebuild the mesh; call the matching Set function to apply it", "number",
			([](Self& c) { return c.cylinderTopRadius; }),
			([](Self& c, float v) { c.cylinderTopRadius = v; }))
		REFLECT_LUA_PROPERTY_CUSTOM("CylinderHeight", "Shape setting - doesn't rebuild the mesh; call the matching Set function to apply it", "number",
			([](Self& c) { return c.cylinderHeight; }),
			([](Self& c, float v) { c.cylinderHeight = v; }))
		REFLECT_LUA_PROPERTY_CUSTOM("CylinderSliceCount", "Shape setting - doesn't rebuild the mesh; call the matching Set function to apply it", "integer",
			([](Self& c) { return c.cylinderSliceCount; }),
			([](Self& c, uint32_t v) { c.cylinderSliceCount = v; }))
		REFLECT_LUA_PROPERTY_CUSTOM("CylinderStackCount", "Shape setting - doesn't rebuild the mesh; call the matching Set function to apply it", "integer",
			([](Self& c) { return c.cylinderStackCount; }),
			([](Self& c, uint32_t v) { c.cylinderStackCount = v; }))
		REFLECT_LUA_PROPERTY_CUSTOM("TorusOuterRadius", "Shape setting - doesn't rebuild the mesh; call the matching Set function to apply it", "number",
			([](Self& c) { return c.torusOuterRadius; }),
			([](Self& c, float v) { c.torusOuterRadius = v; }))
		REFLECT_LUA_PROPERTY_CUSTOM("TorusInnerRadius", "Shape setting - doesn't rebuild the mesh; call the matching Set function to apply it", "number",
			([](Self& c) { return c.torusInnerRadius; }),
			([](Self& c, float v) { c.torusInnerRadius = v; }))
		REFLECT_LUA_PROPERTY_CUSTOM("TorusSliceCount", "Shape setting - doesn't rebuild the mesh; call the matching Set function to apply it", "integer",
			([](Self& c) { return c.torusSliceCount; }),
			([](Self& c, uint32_t v) { c.torusSliceCount = v; }))
		REFLECT_LUA_PROPERTY_CUSTOM("Material", "The material the shape is drawn with", "Material",
			([](Self& c) { return c.material; }),
			([](Self& c, const Ref<Material>& v) { c.material = v; }))
		REFLECT_LUA_PROPERTY_CUSTOM("Mesh", "The generated mesh", "Mesh",
			([](Self& c) { return c.mesh; }),
			([](Self& c, const Ref<Mesh>& v) { c.mesh = v; }))
		REFLECT_LUA_FUNCTION(SetCube, "Rebuild as a cube: (width, depth, height)")
		REFLECT_LUA_FUNCTION(SetSphere, "Rebuild as a sphere: (radius, longitudeLines, latitudeLines)")
		REFLECT_LUA_FUNCTION(SetPlane, "Rebuild as a plane: (width, length, widthLines, lengthLines, tileU, tileV)")
		REFLECT_LUA_FUNCTION(SetCylinder, "Rebuild as a cylinder: (bottomRadius, topRadius, height, sliceCount, stackCount)")
		REFLECT_LUA_FUNCTION(SetCone, "Rebuild as a cone: (bottomRadius, height, sliceCount, stackCount)")
		REFLECT_LUA_FUNCTION(SetTorus, "Rebuild as a torus: (outerRadius, innerRadius, sliceCount)")
		REFLECT_LUA_FUNCTION(SetType, "Rebuild as the given shape type, using its current settings")
	REFLECT_LUA_END()

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
		if (material && !material->GetFilepath().empty() && material != Material::GetDefaultMaterial())
		{
			relativePath = material->GetFilepath().string();
		}
		archive(relativePath);
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

		archive(relativePath);
		if (!relativePath.empty())
		{
			material = AssetManager::GetAsset<Material>(relativePath);
		}
		else
		{
			material = Material::GetDefaultMaterial();
		}
		SetType(type);
	}
};