#pragma once

#include <cstddef>

#include "math/Matrix.h"
#include "math/Plane.h"
#include "Core/BoundingBox.h"

// Six inward facing planes of a camera's view volume, for culling what can't be seen
class Frustum
{
public:
	enum class Side { Left, Right, Bottom, Top, Near, Far };

	// Contains everything, so nothing is culled
	Frustum();
	// From projection * view, as passed to the shaders before transposing
	explicit Frustum(const Matrix4x4& viewProjection);

	bool Contains(const Vector3f& point) const;

	// World space box
	bool Intersects(const BoundingBox& box) const;
	// Box in local space, tested as the world space box enclosing it after the transform
	bool Intersects(const BoundingBox& box, const Matrix4x4& transform) const;
	// Only false when every point is outside the same plane, so shapes larger than the view still pass
	bool Intersects(const Vector3f* points, size_t count) const;

	const Plane& GetPlane(Side side) const { return m_Planes[(size_t)side]; }

private:
	bool IntersectsCentreExtents(const Vector3f& centre, const Vector3f& extents) const;

	// In the order Matrix4x4::FrustumPlanes fills them, which Side matches
	Plane m_Planes[6];
};
