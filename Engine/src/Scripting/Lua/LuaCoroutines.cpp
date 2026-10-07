#include "LuaCoroutines.h"
#include "LuaApiEntry.h"
#include "Logging/Instrumentor.h"

#include <algorithm>

namespace Lua
{
static uint64_t s_Frame = 0;
static uint64_t s_FixedStep = 0;
static uint64_t s_NextCoroutineId = 0;

// Addresses used as markers in the values Wait and WaitFixed yield
static char s_WaitMarker;
static char s_WaitFixedMarker;

static int Wait(lua_State* L)
{
	if (!lua_isyieldable(L))
		return luaL_error(L, "Wait can only be used inside a coroutine started with StartCoroutine");
	lua_Number seconds = luaL_optnumber(L, 1, 0.0);
	lua_pushlightuserdata(L, &s_WaitMarker);
	lua_pushnumber(L, seconds);
	return lua_yield(L, 2);
}

static int WaitFixed(lua_State* L)
{
	if (!lua_isyieldable(L))
		return luaL_error(L, "WaitFixed can only be used inside a coroutine started with StartCoroutine");
	lua_pushlightuserdata(L, &s_WaitFixedMarker);
	return lua_yield(L, 1);
}

uint64_t CoroutineScheduler::Start(lua_State* caller, const sol::function& function, const sol::variadic_args& args)
{
	PROFILE_FUNCTION();
	Ref<Coroutine> coroutine = CreateRef<Coroutine>();
	coroutine->id = ++s_NextCoroutineId;
	coroutine->thread = sol::thread::create(caller);
	m_Coroutines.push_back(coroutine);

	lua_State* thread = coroutine->thread.thread_state();
	function.push(thread);
	int argumentCount = (int)args.size();
	for (int i = 0; i < argumentCount; ++i)
		lua_pushvalue(caller, args.stack_index() + i);
	lua_xmove(caller, thread, argumentCount);

	// Runs straight away, up to its first pause
	Resume(*coroutine, caller, argumentCount);
	return coroutine->id;
}

void CoroutineScheduler::Stop(uint64_t id)
{
	for (Ref<Coroutine>& coroutine : m_Coroutines)
	{
		if (coroutine->id == id)
			coroutine->finished = true;
	}
}

void CoroutineScheduler::Resume(Coroutine& coroutine, lua_State* from, int argumentCount)
{
	coroutine.frame = s_Frame;
	coroutine.fixedStep = s_FixedStep;

	lua_State* thread = coroutine.thread.thread_state();
	int resultCount = 0;
	int status = lua_resume(thread, from, argumentCount, &resultCount);

	if (status == LUA_YIELD)
	{
		// A plain coroutine.yield() waits one frame; Wait and WaitFixed yield a marker first
		coroutine.wakeTime = -1.0;
		coroutine.waitFixed = false;
		if (resultCount >= 1)
		{
			void* marker = lua_touserdata(thread, -resultCount);
			if (marker == &s_WaitMarker && resultCount >= 2)
				coroutine.wakeTime = m_Time + lua_tonumber(thread, -resultCount + 1);
			else if (marker == &s_WaitFixedMarker)
				coroutine.waitFixed = true;
		}
		lua_pop(thread, resultCount);
	}
	else if (status == LUA_OK)
	{
		coroutine.finished = true;
		lua_pop(thread, resultCount);
	}
	else
	{
		m_PendingErrors.push_back(luaL_tolstring(thread, -1, nullptr));
		coroutine.finished = true;
	}
}

void CoroutineScheduler::Update(lua_State* mainState, float deltaTime, bool fixedStep, std::vector<std::string>& errors)
{
	PROFILE_FUNCTION();
	if (m_Coroutines.empty() && m_PendingErrors.empty())
		return;

	if (!fixedStep)
		m_Time += deltaTime;

	// Walk a copy: coroutines may start or stop others while they run
	std::vector<Ref<Coroutine>> coroutines = m_Coroutines;
	for (Ref<Coroutine>& coroutine : coroutines)
	{
		if (coroutine->finished)
			continue;

		if (fixedStep)
		{
			if (coroutine->waitFixed && coroutine->fixedStep < s_FixedStep)
				Resume(*coroutine, mainState, 0);
		}
		else if (!coroutine->waitFixed && coroutine->frame < s_Frame
			&& (coroutine->wakeTime < 0.0 || m_Time >= coroutine->wakeTime))
		{
			Resume(*coroutine, mainState, 0);
		}
	}

	m_Coroutines.erase(std::remove_if(m_Coroutines.begin(), m_Coroutines.end(),
		[](const Ref<Coroutine>& coroutine) { return coroutine->finished; }), m_Coroutines.end());

	errors.insert(errors.end(), m_PendingErrors.begin(), m_PendingErrors.end());
	m_PendingErrors.clear();
}

void BindCoroutines(sol::state& state)
{
	lua_register(state.lua_state(), "Wait", &Wait);
	lua_register(state.lua_state(), "WaitFixed", &WaitFixed);

	RegisterLuaApiEntry({ "StartCoroutine", "StartCoroutine(fn, ...) runs fn as a coroutine owned by this script and returns a handle; it runs straight away until its first pause, then is resumed each frame after OnUpdate", LuaApiEntry::Kind::Function, "", "" });
	RegisterLuaApiEntry({ "StopCoroutine", "StopCoroutine(handle) stops a coroutine started with StartCoroutine", LuaApiEntry::Kind::Function, "", "" });
	RegisterLuaApiEntry({ "Wait", "Wait(seconds) pauses the current coroutine for at least this much game time; coroutine.yield() waits one frame", LuaApiEntry::Kind::Function, "", "" });
	RegisterLuaApiEntry({ "WaitFixed", "WaitFixed() pauses the current coroutine until the next fixed update, after OnFixedUpdate", LuaApiEntry::Kind::Function, "", "" });
}

void InstallCoroutines(sol::environment& environment, const Ref<CoroutineScheduler>& scheduler)
{
	// Weak, so these functions don't keep the scheduler (and its threads) alive after the script is gone
	std::weak_ptr<CoroutineScheduler> weakScheduler = scheduler;

	environment.set_function("StartCoroutine", [weakScheduler](sol::this_state state, sol::function function, sol::variadic_args args) -> sol::optional<uint64_t>
		{
			if (Ref<CoroutineScheduler> scheduler = weakScheduler.lock())
				return scheduler->Start(state, function, args);
			return sol::nullopt;
		});
	environment.set_function("StopCoroutine", [weakScheduler](uint64_t id)
		{
			if (Ref<CoroutineScheduler> scheduler = weakScheduler.lock())
				scheduler->Stop(id);
		});
}

void AdvanceCoroutineFrame()
{
	++s_Frame;
}

void AdvanceCoroutineFixedStep()
{
	++s_FixedStep;
}
}
