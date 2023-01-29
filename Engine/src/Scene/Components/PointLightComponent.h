#pragma once

#include "cereal/access.hpp"
#include "Core/Colour.h"

struct PointLightComponent
{
	Colour colour = Colours::WHITE;
	bool castsShadows = true;
	float range = 10.0f;
	float attenuation = 1.0f;
	PointLightComponent() = default;
	PointLightComponent(const PointLightComponent&) = default;

private:
	friend cereal::access;
	template<typename Archive>
	void serialize(Archive& archive)
	{
		archive(colour, castsShadows, range, attenuation);
	}
};
