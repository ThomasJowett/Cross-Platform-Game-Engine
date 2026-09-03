#pragma once

#include "cereal/cereal.hpp"

#include "math/Vector2f.h"

struct GridLayoutComponent
{
	GridLayoutComponent() = default;
	GridLayoutComponent(const GridLayoutComponent&) = default;

	int columns = 2;
	Vector2f cellSpacing = Vector2f(0.0f, 0.0f);

	float paddingLeft = 0.0f;
	float paddingTop = 0.0f;
	float paddingRight = 0.0f;
	float paddingBottom = 0.0f;

	bool uniformCellSize = true;
	float fixedRowHeight = 0.0f;

private:
	friend cereal::access;
	template<typename Archive>
	void serialize(Archive& archive)
	{
		archive(columns, cellSpacing, paddingLeft, paddingTop, paddingRight, paddingBottom, uniformCellSize, fixedRowHeight);
	}
};
