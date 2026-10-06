#pragma once
#include "sol/sol.hpp"
#include "LuaApiEntry.h"
#include "Core/core.h"

#include "Core/Colour.h"
#include "math/Vector2f.h"
#include "math/Vector3f.h"
#include "math/Vector4f.h"
#include "EnTT/entt.hpp"

#include <cctype>
#include <functional>
#include <stdexcept>
#include <string>
#include <type_traits>

class Scene;
class Entity;

namespace Lua
{
entt::registry& GetSceneRegistry(Scene* scene);

// The calling script's "chunk:line: " prefix, so errors raised from C++ report the Lua line in the error panel
std::string ScriptLocation(lua_State* state);

// The engine's main Lua state, for bindings that can't take sol::this_state (sol counts it as a property argument)
lua_State* MainLuaState();

// Raises a Lua error if the entity was destroyed or never found, naming the function that was called
void CheckEntityValid(const Entity& entity, const char* function, lua_State* state);

// What Lua holds for a component: the entity it belongs to, re-resolved through the registry on every access
template<typename T>
struct ComponentHandle
{
	entt::entity entity = entt::null;
	Scene* scene = nullptr;

	inline static std::string s_LuaName;

	T* TryGet() const
	{
		if (!scene)
			return nullptr;
		entt::registry& registry = GetSceneRegistry(scene);
		return registry.valid(entity) ? registry.try_get<T>(entity) : nullptr;
	}

	T& Get() const
	{
		if (T* component = TryGet())
			return *component;
		throw std::runtime_error(ScriptLocation(MainLuaState()) + s_LuaName + " is no longer valid: its entity was destroyed or the component was removed");
	}
};

namespace Detail
{
template<typename... Args> struct TypeList {};
template<typename T> struct TypeTag {};

template<typename Signature> struct MemberSignature;
template<typename C, typename R, typename... Args>
struct MemberSignature<R(C::*)(Args...)> { using Return = R; using Arguments = TypeList<Args...>; };
template<typename C, typename R, typename... Args>
struct MemberSignature<R(C::*)(Args...) const> { using Return = R; using Arguments = TypeList<Args...>; };
template<typename C, typename R, typename... Args>
struct MemberSignature<R(C::*)(Args...) noexcept> { using Return = R; using Arguments = TypeList<Args...>; };
template<typename C, typename R, typename... Args>
struct MemberSignature<R(C::*)(Args...) const noexcept> { using Return = R; using Arguments = TypeList<Args...>; };

template<typename List> struct DropFirst;
template<typename First, typename... Rest>
struct DropFirst<TypeList<First, Rest...>> { using Type = TypeList<Rest...>; };

template<typename Self, typename F, typename R, typename... Args>
auto MakeResolver(F function, TypeTag<R>, TypeList<Args...>)
{
	return [function](const ComponentHandle<Self>& handle, Args... args) -> std::decay_t<R>
	{
		return std::invoke(function, handle.Get(), std::forward<Args>(args)...);
	};
}
}

// Adapts a binding written against Self& (a member function, or a lambda taking Self& first) to take a
// ComponentHandle<Self>, resolving the live component on each call. Results are returned by value.
template<typename Self, typename F>
auto ResolveOnAccess(F function)
{
	if constexpr (std::is_member_function_pointer_v<F>)
	{
		using Signature = Detail::MemberSignature<F>;
		return Detail::MakeResolver<Self>(function, Detail::TypeTag<typename Signature::Return>{}, typename Signature::Arguments{});
	}
	else
	{
		using Signature = Detail::MemberSignature<decltype(&F::operator())>;
		return Detail::MakeResolver<Self>(function, Detail::TypeTag<typename Signature::Return>{}, typename Detail::DropFirst<typename Signature::Arguments>::Type{});
	}
}
}

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
	static void RegisterLuaBindings(sol::state& state, sol::usertype<Lua::ComponentHandle<Component>>& type) { \
		using Self = Component; \
		static const char* s_ReflectComponentName = #Component;

#define REFLECT_LUA_PROPERTY(Member, Description) \
		type[LuaPropertyName(#Member)] = sol::property( \
			Lua::ResolveOnAccess<Self>([](Self& c) -> decltype(c.Member)& { return c.Member; }), \
			Lua::ResolveOnAccess<Self>([](Self& c, const decltype(c.Member)& v) { c.Member = v; }) \
		); \
		RegisterLuaApiEntry({ LuaPropertyName(#Member), Description, LuaApiEntry::Kind::Property, s_ReflectComponentName, LuaTypeName<decltype(Self::Member)>(), true });

#define REFLECT_LUA_PROPERTY_READONLY(Member, Description) \
		type[LuaPropertyName(#Member)] = sol::property( \
			Lua::ResolveOnAccess<Self>([](Self& c) -> const decltype(c.Member)& { return c.Member; }) \
		); \
		RegisterLuaApiEntry({ LuaPropertyName(#Member), Description, LuaApiEntry::Kind::Property, s_ReflectComponentName, LuaTypeName<decltype(Self::Member)>(), true });

#define REFLECT_LUA_PROPERTY_CUSTOM(Name, Description, LuaType, Getter, Setter) \
		type[Name] = sol::property(Lua::ResolveOnAccess<Self>(Getter), Lua::ResolveOnAccess<Self>(Setter)); \
		RegisterLuaApiEntry({ Name, Description, LuaApiEntry::Kind::Property, s_ReflectComponentName, LuaType, true });

#define REFLECT_LUA_FUNCTION(Method, Description) \
		type.set_function(#Method, Lua::ResolveOnAccess<Self>(&Self::Method)); \
		RegisterLuaApiEntry({ #Method, Description, LuaApiEntry::Kind::Function, s_ReflectComponentName, "", true });

// For functions that need a custom lambda rather than a plain &Self::Method pointer
// (bounds-checked accessors, adapting a different signature, etc).
#define REFLECT_LUA_FUNCTION_CUSTOM(Name, Description, ...) \
		type.set_function(Name, Lua::ResolveOnAccess<Self>(__VA_ARGS__)); \
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

// Registers a global enum table, e.g. BodyType.Dynamic, and documents each value
inline void SetEnum(sol::state& state, const std::string& name, const std::string& description, std::initializer_list<std::pair<sol::string_view, int>> items)
{
	state.new_enum(name, items);
	RegisterLuaApiEntry({ name, description, LuaApiEntry::Kind::Global, "", "" });
	for (const auto& [key, value] : items)
		RegisterLuaApiEntry({ std::string(key), std::to_string(value), LuaApiEntry::Kind::EnumValue, name, "" });
}

template<typename T, typename Property>
void SetProperty(T& type, const std::string& owner, const std::string& name, const std::string& description, const std::string& luaType, Property&& property)
{
	type[name] = std::forward<Property>(property);
	RegisterLuaApiEntry({ name, description, LuaApiEntry::Kind::Property, owner, luaType });
}
}
