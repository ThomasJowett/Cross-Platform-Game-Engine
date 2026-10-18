#include "Frustum.h"

#include <cmath>

Frustum::Frustum()
{
	// A zero normal with positive distance puts every point inside
	for (Plane& plane : m_Planes)
		plane = Plane(0.0f, 0.0f, 0.0f, 1.0f);
}

Frustum::Frustum(const Matrix4x4& viewProjection)
{
	Matrix4x4::FrustumPlanes(m_Planes, viewProjection);
}

bool Frustum::Contains(const Vector3f& point) const
{
	for (const Plane& plane : m_Planes)
	{
		if (plane.SignedDistance(point) < 0.0f)
			return false;
	}
	return true;
}

bool Frustum::Intersects(const BoundingBox& box) const
{
	// An unmeasurable box can't be proven hidden
	if (!box.IsValid())
		return true;

	return IntersectsCentreExtents(box.Center(), (box.Max() - box.Min()) * 0.5f);
}

bool Frustum::Intersects(const BoundingBox& box, const Matrix4x4& transform) const
{
	if (!box.IsValid())
		return true;

	Vector3f centre = transform * box.Center();
	Vector3f localExtents = (box.Max() - box.Min()) * 0.5f;

	// Extents of the world space box around the transformed one (Arvo)
	const auto& m = transform.m;
	Vector3f extents(
		std::abs(m[0][0]) * localExtents.x + std::abs(m[0][1]) * localExtents.y + std::abs(m[0][2]) * localExtents.z,
		std::abs(m[1][0]) * localExtents.x + std::abs(m[1][1]) * localExtents.y + std::abs(m[1][2]) * localExtents.z,
		std::abs(m[2][0]) * localExtents.x + std::abs(m[2][1]) * localExtents.y + std::abs(m[2][2]) * localExtents.z);

	return IntersectsCentreExtents(centre, extents);
}

bool Frustum::Intersects(const Vector3f* points, size_t count) const
{
	for (const Plane& plane : m_Planes)
	{
		bool allOutside = true;
		for (size_t i = 0; i < count; ++i)
		{
			if (plane.SignedDistance(points[i]) >= 0.0f)
			{
				allOutside = false;
				break;
			}
		}

		if (allOutside)
			return false;
	}
	return true;
}

bool Frustum::IntersectsCentreExtents(const Vector3f& centre, const Vector3f& extents) const
{
	for (const Plane& plane : m_Planes)
	{
		// How far the box reaches towards the plane's normal
		float radius = std::abs(plane.a) * extents.x + std::abs(plane.b) * extents.y + std::abs(plane.c) * extents.z;
		if (plane.SignedDistance(centre) + radius < 0.0f)
			return false;
	}
	return true;
}
