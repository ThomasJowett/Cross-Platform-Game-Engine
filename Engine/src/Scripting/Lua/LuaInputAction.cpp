#include "LuaBindings.h"

#include "Logging/Instrumentor.h"
#include "LuaManager.h"

#include "Core/InputActionSystem.h"

namespace Lua
{
void BindInputAction(sol::state& state)
{
	PROFILE_FUNCTION();
	sol::table inputAction = state.create_table();
	sol::table inputActionMeta = state.create_table();
	inputActionMeta[sol::meta_function::index] = [](sol::this_state s, sol::table, std::string name) -> sol::table
		{
			sol::state_view lua(s);
			sol::table accessor = lua.create_table();

			accessor.set_function("IsTriggered", [name]()
				{
					return InputActionSystem::IsActionTriggered(name);
				});
			accessor.set_function("GetValue", [name]()
				{
					return InputActionSystem::GetActionValue(name).value.x;
				});
			accessor.set_function("GetAxis2D", [name]()
				{
					return InputActionSystem::GetActionValue(name).value;
				});

			return accessor;
		};
	inputAction[sol::metatable_key] = inputActionMeta;
	state["InputAction"] = inputAction;
	LuaManager::AddIdentifier("InputAction", "Access an input action by name, e.g. InputAction.Attack:IsTriggered()");
	// The accessors are made per action name on lookup, so they're documented here rather than where they're bound
	RegisterLuaApiEntry({ "IsTriggered", "InputAction.<Name>:IsTriggered(): whether the action fired this frame", LuaApiEntry::Kind::Function, "InputAction", "" });
	RegisterLuaApiEntry({ "GetValue", "InputAction.<Name>:GetValue(): the action's value as a number, e.g. a trigger or a 1D axis", LuaApiEntry::Kind::Function, "InputAction", "" });
	RegisterLuaApiEntry({ "GetAxis2D", "InputAction.<Name>:GetAxis2D(): the action's value as a Vec2, e.g. a stick or WASD", LuaApiEntry::Kind::Function, "InputAction", "" });
}
}
