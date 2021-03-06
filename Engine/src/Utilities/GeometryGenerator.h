#pragma once

#include "Renderer/VertexArray.h"

namespace GeometryGenerator
{
	/**
	 * Create a Cube Vertex array
	 * 
	 * @param width 
	 * @param height 
	 * @param depth 
	 * @return Ref<VertexArray> 
	 */
	Ref<VertexArray> CreateCube(float width, float height, float depth);

	/**
	 * Create a Lat/Long Sphere Vertex array
	 * 
	 * @param radius 
	 * @param longitudeLines 
	 * @param latitudeLines 
	 * @return Ref<VertexArray> 
	 */
	Ref<VertexArray> CreateSphere(float radius, uint32_t longitudeLines, uint32_t latitudeLines);

	/**
	 * Create a Gridded Plane Vertex array
	 * 
	 * @param width size of grid
	 * @param length size of grid
	 * @param widthLines number of width lines of grid
	 * @param lengthLines number of length lines of grid
	 * @param tileU tiling of texture on grid
	 * @param tileV tiling of texture on grid
	 * @return Ref<VertexArray> 
	 */
	Ref<VertexArray> CreateGrid(float width, float length, unsigned int widthLines, unsigned int lengthLines, float tileU, float tileV);

	/**
	 * Create a Full Screen Quad used for post processing
	 * 
	 * @return Ref<VertexArray> 
	 */
	Ref<VertexArray> CreateFullScreenQuad();

	/**
	 * Create a Cylinder Vertex array
	 * 
	 * @param bottomRadius 
	 * @param topRadius 
	 * @param height 
	 * @param sliceCount number of lines along the cylinder length
	 * @param stackCount number of lines around cylinder
	 * @return Ref<VertexArray> 
	 */
	Ref<VertexArray> CreateCylinder(float bottomRadius, float topRadius, float height, uint32_t sliceCount, uint32_t stackCount);

	/**
	 * Create a Torus Vertex array
	 * 
	 * @param diameter of main ring
	 * @param thickness inner diameter
	 * @param segments 
	 * @return Ref<VertexArray> 
	 */
	Ref<VertexArray> CreateTorus(float diameter, float thickness, uint32_t segments);
}