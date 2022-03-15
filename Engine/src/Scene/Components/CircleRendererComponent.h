#pragma once

#include "Core/Colour.h"

struct CircleRendererComponent
{
	Colour colour{ 1.0f, 1.0f, 1.0f, 1.0f };
	float Radius = 0.5f;
	float Thickness = 1.0f;
	float Fade = 0.005f;

	CircleRendererComponent() = default;
	CircleRendererComponent(const CircleRendererComponent&) = default;
private:
	friend cereal::access;
	template<typename Archive>
	void serialize(Archive& archive)
	{
		archive(colour, Radius, Thickness, Fade);
	}

};