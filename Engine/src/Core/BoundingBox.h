#pragma once

#include "math/Vector3f.h"

class BoundingBox
{
public:
	BoundingBox() = default;
	BoundingBox(const Vector3f& min, const Vector3f& max);

	void Merge(const BoundingBox& other);
	void Merge(const Vector3f& point);

	Vector3f Center() const;
	Vector3f Min() const { return m_Min; };
	Vector3f Max() const { return m_Max; };
private:
	Vector3f m_Min;
	Vector3f m_Max;
};