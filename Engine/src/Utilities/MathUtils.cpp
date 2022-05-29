#include "stdafx.h"
#include "MathUtils.h"

Line3D MathUtils::ComputeCameraRay(const Matrix4x4& viewMat, const Matrix4x4& projectionMat, const Vector2f& screenPosition, const Vector2f& viewportSize)
{
	Line3D ray;
	Matrix4x4 viewProjInverse = Matrix4x4::Inverse(projectionMat * viewMat);

	float x = ((screenPosition.x) / viewportSize.x) * 2.0f - 1.0f;
	float y = (1.0f - (screenPosition.y) / viewportSize.y) * 2.0f - 1.0f;

	Vector4f rayOrigin = viewProjInverse * Vector4f(x, y, 0.0f, 1.0f);
	rayOrigin /= rayOrigin.w;

	Vector4f rayEnd = viewProjInverse * Vector4f(x, y, 1.0f - FLT_EPSILON, 1.0f);
	rayEnd /= rayEnd.w;

	ray.p = Vector3f(rayOrigin.x, rayOrigin.y, rayOrigin.z);

	ray.d = Vector3f(rayEnd.x, rayEnd.y, rayEnd.z) - ray.p;
	ray.d.Normalize();

	return ray;
}

Vector3f MathUtils::WorldToScreenSpace(const Matrix4x4& viewMat, const Matrix4x4& projectionMat, const Vector3f& worldPosition, const Vector2f& viewportSize)
{
	Vector4f trans = projectionMat * Matrix4x4::Inverse(viewMat) * Vector4f(worldPosition, 1.0f);

	trans *= 0.5f / trans.w;
	trans += Vector4f(0.5f, 0.5f, 0.5f, 0.0f);
	trans.y = 1.0f - trans.y;
	trans.x *= viewportSize.x;
	trans.y *= viewportSize.y;
	trans.x += worldPosition.x;
	trans.y += worldPosition.y;
	trans.z += worldPosition.z;
	return Vector3f(trans.x, trans.y, trans.z);
}
