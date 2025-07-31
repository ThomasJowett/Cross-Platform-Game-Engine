#pragma once

#include "Renderer/Mesh.h"

namespace GeometryGenerator
{
// Create a Cube mesh
Ref<Mesh> CreateCube(float width, float height, float depth);

// Create a Lat/Long Sphere mesh
Ref<Mesh> CreateSphere(float radius, uint32_t longitudeLines, uint32_t latitudeLines);

// Create a Gridded Plane mesh
Ref<Mesh> CreateGrid(float width, float length, uint32_t widthLines, uint32_t lengthLines, float tileU, float tileV);

// Create a Full Screen Quad used for post processing
Ref<Mesh> CreateFullScreenQuad();

// Create a Cylinder mesh
Ref<Mesh> CreateCylinder(float bottomRadius, float topRadius, float height, uint32_t sliceCount, uint32_t stackCount);

// Create a Torus mesh
Ref<Mesh> CreateTorus(float diameter, float thickness, uint32_t segments);

std::vector<float> FlattenVertices(const std::vector<Vertex>& vertices);
}