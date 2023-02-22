#pragma once
#include <vector>
#include "math/Vector2f.h"

namespace Triangulation
{
	bool Triangulate(const std::vector<Vector2f>& vertices, std::vector<uint32_t>& triangles);

	bool IsPointInTriangle(const Vector2f& p, const Vector2f& a, const Vector2f& b, const Vector2f& c);

	bool IsSimplePolygon(const std::vector<Vector2f>& vertices);

	bool ContainsColinearEdges(const std::vector<Vector2f>& vertices);

	bool IsClockwiseWinding(const std::vector<Vector2f>& vertices);
}
