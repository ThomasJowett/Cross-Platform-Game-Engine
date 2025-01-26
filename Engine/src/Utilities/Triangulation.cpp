#include "stdafx.h"
#include "Triangulation.h"
#include "Logging/Instrumentor.h"

template<typename T>
T GetItemLooped(const std::vector<T>& vector, int index)
{
	if (index >= (int)vector.size())
	{
		return vector.at(index % vector.size());
	}
	else if (index < 0)
	{
		return vector.at((index % (int)vector.size()) + vector.size());
	}
	else
	{
		return vector.at(index);
	}
}

bool Triangulation::Triangulate(const std::vector<Vector2f>& vertices, std::vector<uint32_t>& triangles)
{
	if (vertices.size() < 3)
	{
		ENGINE_ERROR("Could not triangulate polygon! Not enough vertices.");
		return false;
	}

	if (!IsSimplePolygon(vertices))
	{
		ENGINE_ERROR("Could not triangulate polygon! Not a simple polygon.");
	}

	if (ContainsColinearEdges(vertices))
	{
		ENGINE_ERROR("Could not triangulate polygon! Vertices contains colinear edges.");
	}

	std::vector<uint32_t> indexList(vertices.size());
	std::iota(indexList.begin(), indexList.end(), 0);

	bool isClockwiseWinding = IsClockwiseWinding(vertices);
	if (!isClockwiseWinding)
	{
		std::reverse(indexList.begin(), indexList.end());
	}

	uint32_t totalTriangleCount = (uint32_t)vertices.size() - 2;
	uint32_t totalTriangleIndexCount = totalTriangleCount * 3;

	triangles.resize(totalTriangleIndexCount);
	uint32_t triangleIndexCount = 0;

	while (indexList.size() > 3)
	{
		for (int i = 0; i < (int)indexList.size(); i++)
		{
			uint32_t currentIndex = indexList[i];
			uint32_t previousIndex = GetItemLooped<uint32_t>(indexList, i - 1);
			uint32_t nextIndex = GetItemLooped<uint32_t>(indexList, i + 1);

			if (Vector2f::Cross(vertices[previousIndex] - vertices[currentIndex], vertices[nextIndex] - vertices[currentIndex]) < 0.0f)
			{
				continue;
			}

			bool isEar = true;

			for (size_t j = 0; j < vertices.size(); j++)
			{
				if (j == currentIndex || j == previousIndex || j == nextIndex)
				{
					continue;
				}

				if (IsPointInTriangle(vertices[j], vertices[previousIndex], vertices[currentIndex], vertices[nextIndex]))
				{
					isEar = false;
					break;
				}
			}

			if (isEar)
			{
				triangles[triangleIndexCount++] = previousIndex;
				triangles[triangleIndexCount++] = currentIndex;
				triangles[triangleIndexCount++] = nextIndex;

				indexList.erase(indexList.begin() + i);
				break;
			}
		}
	}

	triangles[triangleIndexCount++] = indexList[0];
	triangles[triangleIndexCount++] = indexList[1];
	triangles[triangleIndexCount++] = indexList[2];
	if (!isClockwiseWinding)
	{
		std::reverse(triangles.begin(), triangles.end());
	}
	return true;
}

bool Triangulation::IsPointInTriangle(const Vector2f& p, const Vector2f& a, const Vector2f& b, const Vector2f& c)
{
	float cross1 = Vector2f::Cross(b - a, p - a);
	float cross2 = Vector2f::Cross(c - b, p - b);
	float cross3 = Vector2f::Cross(a - c, p - c);

	if (cross1 > 0.0f || cross2 > 0.0f || cross3 > 0.0f)
	{
		return false;
	}
	return true;
}

bool Triangulation::IsSimplePolygon(const std::vector<Vector2f>& vertices)
{
	return true;
}

bool Triangulation::ContainsColinearEdges(const std::vector<Vector2f>& vertices)
{
	return false;
}

bool Triangulation::IsClockwiseWinding(const std::vector<Vector2f>& vertices)
{
	float totalArea = 0.0f;

	for (int i = 0; i < vertices.size(); i++)
	{
		Vector2f a = vertices[i];
		Vector2f b = vertices[(i + 1) % vertices.size()];

		float dy = (a.y + b.y) / 2.0f;
		float dx = b.x - a.x;

		float area = dy * dx;
		totalArea += area;
	}

	return totalArea > 0.0f;
}
