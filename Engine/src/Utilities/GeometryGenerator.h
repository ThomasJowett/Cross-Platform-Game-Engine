#pragma once

#include "Renderer/VertexArray.h"

namespace GeometryGenerator
{
	Ref<VertexArray> CreateCube(float width, float height, float depth);

	Ref<VertexArray> CreateSphere(float radius, unsigned int longitudeLines, unsigned int latitudeLines);

	Ref<VertexArray> CreateGrid(float width, float length, unsigned int widthLines, unsigned int lengthLines, float tileU, float tileV);

	Ref<VertexArray> CreateFullScreenQuad();
}