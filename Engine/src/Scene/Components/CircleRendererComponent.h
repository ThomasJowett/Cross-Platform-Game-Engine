#pragma once

#include "Core/Colour.h"
#include "Scripting/Lua/LuaBindings.h"

struct CircleRendererComponent
{
	Colour colour{ 1.0f, 1.0f, 1.0f, 1.0f };
	float radius = 0.5f;
	float thickness = 1.0f;
	float fade = 0.005f;

	CircleRendererComponent() = default;
	CircleRendererComponent(const CircleRendererComponent&) = default;
	REFLECT_LUA_BEGIN(CircleRendererComponent)
		REFLECT_LUA_PROPERTY_CUSTOM("Colour", "Colour of the circle", "Colour",
			([](Self& c) -> Colour& { return c.colour; }),
			([](Self& c, const Colour& v) { c.colour = v; }))
		REFLECT_LUA_PROPERTY_CUSTOM("Radius", "Radius of the circle", "number",
			([](Self& c) { return c.radius; }),
			([](Self& c, float v) { c.radius = v; }))
		REFLECT_LUA_PROPERTY_CUSTOM("Thickness", "1 draws a filled circle; smaller values draw a ring", "number",
			([](Self& c) { return c.thickness; }),
			([](Self& c, float v) { c.thickness = v; }))
		REFLECT_LUA_PROPERTY_CUSTOM("Fade", "Softness of the circle's edge", "number",
			([](Self& c) { return c.fade; }),
			([](Self& c, float v) { c.fade = v; }))
	REFLECT_LUA_END()

private:
	friend cereal::access;
	template<typename Archive>
	void serialize(Archive& archive)
	{
		archive(colour, radius, thickness, fade);
	}

};