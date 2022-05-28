#include "stdafx.h"
#include "MathUtils.h"

Line3D MathUtils::ComputeCameraRay(const Matrix4x4& viewMat, const Matrix4x4& projectionMat, const Vector2f& screenPosition, const Vector2f& viewportSize)
{
	Line3D ray;
	Matrix4x4 viewProjInverse = Matrix4x4::Inverse(projectionMat * viewMat);

	float x = (2.0f * (screenPosition.x - viewportSize.x * 0.5f) - 1.0f) / viewportSize.x;
	float y = (2.0f * -(screenPosition.y - viewportSize.y * 0.5f) - 1.0f) / viewportSize.y;

	Vector4f nearPlane = viewProjInverse * Vector4f(x, y, 0.0f, 1.0f);
	nearPlane /= nearPlane.w;

	Vector4f farPlane = viewProjInverse * Vector4f(x, y, 1.0f, 1.0f);
	farPlane /= farPlane.w;

	ray.p = Vector3f(nearPlane.x, nearPlane.y, nearPlane.z);

	ray.d = Vector3f(farPlane.x, farPlane.y, farPlane.z) - ray.p;
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
