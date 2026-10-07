#pragma once

#include "sol/sol.hpp"
#include "Core/core.h"

#include <string>
#include <vector>

namespace Lua
{
// One script's coroutines, started with StartCoroutine and resumed by its LuaScriptComponent
class CoroutineScheduler
{
public:
	uint64_t Start(lua_State* caller, const sol::function& function, const sol::variadic_args& args);
	void Stop(uint64_t id);

	// Resume the coroutines that are due; errors raised since the last call are appended to errors
	void Update(lua_State* mainState, float deltaTime, bool fixedStep, std::vector<std::string>& errors);

private:
	struct Coroutine
	{
		uint64_t id = 0;
		sol::thread thread;
		double wakeTime = -1.0;
		bool waitFixed = false;
		uint64_t frame = 0;
		uint64_t fixedStep = 0;
		bool finished = false;
	};

	void Resume(Coroutine& coroutine, lua_State* from, int argumentCount);

	std::vector<Ref<Coroutine>> m_Coroutines;
	std::vector<std::string> m_PendingErrors;
	double m_Time = 0.0;
};

// Binds Wait and WaitFixed, and documents the coroutine functions
void BindCoroutines(sol::state& state);

// Defines StartCoroutine and StopCoroutine in a script's environment, acting on that script's scheduler
void InstallCoroutines(sol::environment& environment, const Ref<CoroutineScheduler>& scheduler);

// Advance the counters used to resume each coroutine at most once per update
void AdvanceCoroutineFrame();
void AdvanceCoroutineFixedStep();
}
