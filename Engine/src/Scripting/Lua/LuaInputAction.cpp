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
}
}
