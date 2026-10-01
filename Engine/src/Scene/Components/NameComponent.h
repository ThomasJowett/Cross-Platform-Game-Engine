#pragma once
#include <string>

#include "cereal/cereal.hpp"
#include "Scripting/Lua/LuaBindings.h"

struct NameComponent
{
	std::string name;

	NameComponent() = default;
	NameComponent(const NameComponent& name) = default;
	NameComponent(const std::string& name)
		:name(name) {}

	operator std::string& () { return name; }
	operator const std::string& () const { return name; }

	REFLECT_LUA_BEGIN(NameComponent)
		REFLECT_LUA_PROPERTY(name, "The name of this entity")
	REFLECT_LUA_END()
private:
	friend cereal::access;
	template<typename Archive>
	void serialize(Archive& archive)
	{
		archive(name);
	}
};