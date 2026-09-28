#pragma once

#include "cereal/cereal.hpp"

struct StackLayoutComponent
{
	StackLayoutComponent() = default;
	StackLayoutComponent(const StackLayoutComponent&) = default;

	bool horizontal = false;

	float spacing = 0.0f;
	bool stretchCrossAxis = true;

	float paddingLeft = 0.0f;
	float paddingTop = 0.0f;
	float paddingRight = 0.0f;
	float paddingBottom = 0.0f;

private:
	friend cereal::access;
	template<typename Archive>
	void serialize(Archive& archive)
	{
		archive(horizontal, spacing, stretchCrossAxis, paddingLeft, paddingTop, paddingRight, paddingBottom);
	}
};
