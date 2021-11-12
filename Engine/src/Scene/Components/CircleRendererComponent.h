#pragma once

#include "Core/Colour.h"

struct CircleRendererComponent
{
	Colour Colour{ 1.0f, 1.0f, 1.0f, 1.0f };
	float Radius = 0.5f;
	float Thickness = 1.0f;
	float Fade = 0.005f;

	CircleRendererComponent() = default;
	CircleRendererComponent(const CircleRendererComponent&) = default;

	template<typename Archive>
	void serialize(Archive& archive)
	{
		archive(Colour, Radius, Thickness, Fade);
	}

};