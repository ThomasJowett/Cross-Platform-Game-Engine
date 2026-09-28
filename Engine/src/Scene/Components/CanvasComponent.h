#pragma once

#include "cereal/cereal.hpp"
#include "Scripting/Lua/LuaBindings.h"

struct CanvasComponent
{
	CanvasComponent() = default;
	CanvasComponent(const CanvasComponent&) = default;

	float pixelPerUnit = 1.0f;

	REFLECT_LUA_BEGIN(CanvasComponent)
		REFLECT_LUA_PROPERTY(pixelPerUnit, "Pixels per world unit this canvas's UI is drawn at")
	REFLECT_LUA_END()

private:
	friend cereal::access;
	template<typename Archive>
	void serialize(Archive& archive)
	{
		archive(pixelPerUnit);
	}
};
