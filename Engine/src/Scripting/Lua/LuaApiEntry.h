#pragma once
#include <string>

// Kept free of LuaManager.h (and its Entity/Scene includes) so component headers can use
// the REFLECT_LUA_* macros without an include cycle.
struct LuaApiEntry
{
	enum class Kind { Global, Property, Function, ComponentAccessor };

	std::string name;
	std::string description;
	Kind kind = Kind::Global;
	std::string component;
	std::string type;
	bool isComponent = false;
};

// Same as LuaManager::AddApiEntry
void RegisterLuaApiEntry(LuaApiEntry entry);
