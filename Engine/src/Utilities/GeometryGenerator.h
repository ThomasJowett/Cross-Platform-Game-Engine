#pragma once

#include "Renderer/VertexArray.h"

namespace GeometryGenerator
{
	Ref<VertexArray> CreateCube(float width, float height, float depth);

	Ref<VertexArray> CreateSphere(float radius, uint32_t longitudeLines, uint32_t latitudeLines);

	Ref<VertexArray> CreateGrid(float width, float length, unsigned int widthLines, unsigned int lengthLines, float tileU, float tileV);

	Ref<VertexArray> CreateFullScreenQuad();

	Ref<VertexArray> CreateCylinder(float bottomRadius, float topRadius, float height, uint32_t sliceCount, uint32_t stackCount);

	Ref<VertexArray> CreateTorus(float diameter, float thickness, uint32_t segments);
}