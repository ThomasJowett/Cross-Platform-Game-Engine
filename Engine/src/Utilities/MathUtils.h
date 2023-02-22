#pragma once

#include "math/Vector3f.h"
#include "math/Matrix.h"

namespace MathUtils
{
	Line3D ComputeCameraRay(const Matrix4x4& viewMat, const Matrix4x4& projectionMat, const Vector2f& screenPosition, const Vector2f& viewportSize);
	Vector3f WorldToScreenSpace(const Matrix4x4& viewMat, const Matrix4x4& projectionMat, const Vector3f& worldPosition, const Vector2f& viewportSize);
}
