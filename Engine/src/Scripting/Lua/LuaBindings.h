#pragma once
#include "sol/sol.hpp"
#include "LuaApiEntry.h"
#include "Core/core.h"

#include "Core/Colour.h"
#include "math/Vector2f.h"
#include "math/Vector3f.h"
#include "math/Vector4f.h"

#include <cctype>
#include <string>
#include <type_traits>

// Lua-facing type name for a reflected property, used by ScriptView's autocomplete and
// the Lua API doc generator. Primitive/enum types resolve automatically; anything else
// falls back to LuaTypeNameTrait, which defaults to "unknown" until given a friendlier
// name via LUA_TYPE_NAME (e.g. for asset Ref<T> types or other engine structs).
template<typename T>
struct LuaTypeNameTrait
{
	static std::string Get() { return "unknown"; }
};

#define LUA_TYPE_NAME(CppType, LuaName) \
	template<> struct LuaTypeNameTrait<CppType> { static std::string Get() { return LuaName; } };

template<typename T>
struct LuaTypeNameTrait<Ref<T>>
{
	static std::string Get() { return LuaTypeNameTrait<T>::Get(); }
};

LUA_TYPE_NAME(std::string, "string")
LUA_TYPE_NAME(Colour, "Colour")
LUA_TYPE_NAME(Vector2f, "Vec2")
LUA_TYPE_NAME(Vector3f, "Vec3")
LUA_TYPE_NAME(Vector4f, "Vector4f")

template<typename T>
std::string LuaTypeName()
{
	using Decayed = std::decay_t<T>;
	if constexpr (std::is_same_v<Decayed, bool>)
		return "boolean";
	else if constexpr (std::is_floating_point_v<Decayed>)
		return "number";
	else if constexpr (std::is_enum_v<Decayed>)
		return "integer (enum)";
	else if constexpr (std::is_integral_v<Decayed>)
		return "integer";
	else
		return LuaTypeNameTrait<Decayed>::Get();
}

// Lua property names are PascalCase, like functions, whatever the C++ member is called
inline std::string LuaPropertyName(const char* member)
{
	std::string name = member;
	if (!name.empty())
		name[0] = (char)std::toupper((unsigned char)name[0]);
	return name;
}

// Reflection macros for automatic Lua bindings registration inside components. A
// description is required on every property/function so the same declaration doubles as
// documentation - see LuaApiEntry (LuaApiEntry.h) for what this feeds into.
#define REFLECT_LUA_BEGIN(Component) \
	static void RegisterLuaBindings(sol::state& state, sol::usertype<Component>& type) { \
		using Self = Component; \
		static const char* s_ReflectComponentName = #Component;

#define REFLECT_LUA_PROPERTY(Member, Description) \
		type[LuaPropertyName(#Member)] = sol::property( \
			[](Self& c) -> decltype(c.Member)& { return c.Member; }, \
			[](Self& c, const decltype(c.Member)& v) { c.Member = v; } \
		); \
		RegisterLuaApiEntry({ LuaPropertyName(#Member), Description, LuaApiEntry::Kind::Property, s_ReflectComponentName, LuaTypeName<decltype(Self::Member)>(), true });

#define REFLECT_LUA_PROPERTY_READONLY(Member, Description) \
		type[LuaPropertyName(#Member)] = sol::property( \
			[](Self& c) -> const decltype(c.Member)& { return c.Member; } \
		); \
		RegisterLuaApiEntry({ LuaPropertyName(#Member), Description, LuaApiEntry::Kind::Property, s_ReflectComponentName, LuaTypeName<decltype(Self::Member)>(), true });

#define REFLECT_LUA_PROPERTY_CUSTOM(Name, Description, LuaType, Getter, Setter) \
		type[Name] = sol::property(Getter, Setter); \
		RegisterLuaApiEntry({ Name, Description, LuaApiEntry::Kind::Property, s_ReflectComponentName, LuaType, true });

#define REFLECT_LUA_FUNCTION(Method, Description) \
		type.set_function(#Method, &Self::Method); \
		RegisterLuaApiEntry({ #Method, Description, LuaApiEntry::Kind::Function, s_ReflectComponentName, "", true });

// For functions that need a custom lambda rather than a plain &Self::Method pointer
// (bounds-checked accessors, adapting a different signature, etc).
#define REFLECT_LUA_FUNCTION_CUSTOM(Name, Description, ...) \
		type.set_function(Name, __VA_ARGS__); \
		RegisterLuaApiEntry({ Name, Description, LuaApiEntry::Kind::Function, s_ReflectComponentName, "", true });

#define REFLECT_LUA_END() \
	}

namespace Lua
{
void BindLogging(sol::state& state);
void BindApp(sol::state& state);
void BindScene(sol::state& state);
void BindEntity(sol::state& state);
void BindInput(sol::state& state);
void BindInputAction(sol::state& state);
void BindMath(sol::state& state);
void BindCommonTypes(sol::state& state);
void BindDebug(sol::state& state);
void BindSignaling(sol::state& state);
void BindPathfinding(sol::state& state);

template<typename T, typename... Args>
void SetFunction(T& type, const std::string& owner, const std::string& name, const std::string& description, Args&&... args)
{
	type.set_function(name, std::forward<Args>(args)...);
	RegisterLuaApiEntry({ name, description, LuaApiEntry::Kind::Function, owner, "" });
}

template<typename T, typename Property>
void SetProperty(T& type, const std::string& owner, const std::string& name, const std::string& description, const std::string& luaType, Property&& property)
{
	type[name] = std::forward<Property>(property);
	RegisterLuaApiEntry({ name, description, LuaApiEntry::Kind::Property, owner, luaType });
}
}
