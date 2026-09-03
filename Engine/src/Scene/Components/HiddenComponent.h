#pragma once

#include "cereal/cereal.hpp"
#include "Scripting/Lua/LuaBindings.h"

struct HiddenComponent
{
	HiddenComponent() = default;
	HiddenComponent(const HiddenComponent&) = default;

	bool hidden = true;

	REFLECT_LUA_BEGIN(HiddenComponent)
		REFLECT_LUA_PROPERTY(hidden, "Whether this entity is hidden from rendering - a hidden parent also hides its children, but doesn't change their own hidden value")
	REFLECT_LUA_END()

private:
	friend cereal::access;
	template<typename Archive>
	void serialize(Archive& archive)
	{
		archive(hidden);
	}
};
