#pragma once

#include "math/Vector3f.h"

struct AABB
{
	AABB() = default;
	AABB(const Vector3f& min, const Vector3f& max);

	Vector3f min;
	Vector3f max;
};