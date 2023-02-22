#pragma once

#include "math/Vector3f.h"

class BoundingBox
{
public:
	BoundingBox();
	BoundingBox(const Vector3f& min, const Vector3f& max);
	BoundingBox(const BoundingBox& other) = default;

	void EnclosePoints(const float* vertices, uint32_t vertexCount, int stride);

	void Merge(const BoundingBox& other);
	void Merge(const Vector3f& point);

	Vector3f Center() const;
	Vector3f Min() const { return m_Min; };
	Vector3f Max() const { return m_Max; };

	void Invalidate();
	bool IsValid() const;
private:
	Vector3f m_Min;
	Vector3f m_Max;
};