#pragma once

#include "cereal/cereal.hpp"

struct CanvasComponent
{
	CanvasComponent() = default;
	CanvasComponent(const CanvasComponent&) = default;

	float pixelPerUnit = 1.0f;

private:
	friend cereal::access;
	template<typename Archive>
	void serialize(Archive& archive)
	{
		archive(pixelPerUnit);
	}
};
