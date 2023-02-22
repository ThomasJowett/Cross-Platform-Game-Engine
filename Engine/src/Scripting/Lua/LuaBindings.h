#pragma once
#include "sol/sol.hpp"

namespace Lua
{
void BindLogging(sol::state& state);
void BindApp(sol::state& state);
void BindScene(sol::state& state);
void BindEntity(sol::state& state);
void BindInput(sol::state& state);
void BindMath(sol::state& state);
void BindCommonTypes(sol::state& state);
void BindDebug(sol::state& state);
}
