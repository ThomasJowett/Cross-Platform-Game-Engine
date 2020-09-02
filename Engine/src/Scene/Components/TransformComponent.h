#pragma once

#include "math/Matrix.h"

struct TransformComponent
{
	Matrix4x4 Transform;

	TransformComponent() = default;
	TransformComponent(const TransformComponent&) = default;
	TransformComponent(const Matrix4x4 & transform)
		:Transform(transform) {}

	operator Matrix4x4& () { return Transform; }
	operator const Matrix4x4& () const { return Transform; }
};