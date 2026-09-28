#pragma once

#include "math/Vector3f.h"
#include "math/Matrix.h"

namespace MathUtils
{
	Line3D ComputeCameraRay(const Matrix4x4& viewMat, const Matrix4x4& projectionMat, const Vector2f& screenPosition, const Vector2f& viewportSize);
	Vector3f WorldToScreenSpace(const Matrix4x4& viewMat, const Matrix4x4& projectionMat, const Vector3f& worldPosition, const Vector2f& viewportSize);

	// Unprojects a screen-pixel coordinate (origin top-left, Y down) to a point in world space, on the
	// worldZ plane - the inverse of WorldToScreenSpace. viewMat/projectionMat use the same convention
	// as ComputeCameraRay (viewMat already inverted from the camera's world transform).
	Vector3f ScreenToWorldPoint(const Matrix4x4& viewMat, const Matrix4x4& projectionMat, const Vector2f& screenPosition, const Vector2f& viewportSize, float worldZ = 0.0f);
}
