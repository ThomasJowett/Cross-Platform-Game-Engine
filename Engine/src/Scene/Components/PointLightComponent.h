#pragma once

#include "cereal/access.hpp"
#include "Core/Colour.h"

struct PointLightComponent
{
	Colour colour;
	bool castsShadows = true;
	PointLightComponent() = default;

private:
	friend cereal::access;
	template<typename Archive>
	void serialize(Archive& archive)
	{
		archive(colour, castsShadows);
	}
};
