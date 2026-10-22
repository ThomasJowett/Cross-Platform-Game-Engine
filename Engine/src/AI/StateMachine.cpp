#include "StateMachine.h"

#include "Core/Application.h"
#include "Logging/Instrumentor.h"
#include "Scene/AssetManager.h"
#include "Scene/SceneManager.h"
#include "Scripting/Lua/LuaErrorEvent.h"
#include "Scripting/Lua/LuaManager.h"

namespace
{
// Caps chained changes (a state changing again from OnStateEntry) so a loop can't hang the game
constexpr int c_MaxTransitionsPerChange = 16;
}

LuaState::LuaState(const std::string& name, const std::filesystem::path& filepath)
	:State(name), m_ScriptPath(filepath)
{
	PROFILE_FUNCTION();

	if (!filepath.empty())
		m_LuaScript = AssetManager::GetAsset<LuaScript>(filepath);
}

LuaState::~LuaState()
{
}

bool LuaState::Bind(Entity entity, StateMachine* stateMachine)
{
	PROFILE_FUNCTION();

	m_SolEnvironment.reset();
	m_OnStateEntryFunc.reset();
	m_OnStateUpdateFunc.reset();
	m_OnStateExitFunc.reset();

	if (!m_LuaScript)
	{
		if (!m_ScriptPath.empty())
			ENGINE_ERROR("State '{0}' could not load lua script {1}", GetName(), m_ScriptPath);
		return false;
	}

	sol::state& lua = LuaManager::GetState();
	m_SolEnvironment = CreateRef<sol::environment>(lua, sol::create, lua.globals());

	(*m_SolEnvironment)["CurrentScene"] = SceneManager::CurrentScene();
	(*m_SolEnvironment)["CurrentEntity"] = entity;

	// Weak so a script holding on to these can't keep the state machine alive
	std::weak_ptr<StateMachine> weakStateMachine = stateMachine->weak_from_this();
	sol::table stateMachineTable = lua.create_table();
	stateMachineTable.set_function("ChangeState", [weakStateMachine](const std::string& name)
		{
			if (Ref<StateMachine> machine = weakStateMachine.lock())
				return machine->ChangeState(name);
			return false;
		});
	stateMachineTable.set_function("GetCurrentStateName", [weakStateMachine]()
		{
			Ref<StateMachine> machine = weakStateMachine.lock();
			return machine ? machine->GetCurrentStateName() : std::string();
		});
	stateMachineTable.set_function("GetPreviousStateName", [weakStateMachine]()
		{
			Ref<StateMachine> machine = weakStateMachine.lock();
			return machine ? machine->GetPreviousStateName() : std::string();
		});
	(*m_SolEnvironment)["StateMachine"] = stateMachineTable;

	sol::protected_function_result result = lua.script(m_LuaScript->GetSource(), *m_SolEnvironment, sol::script_pass_on_error);

	if (!result.valid())
	{
		sol::error error = result;

		auto event = LuaErrorEvent(m_ScriptPath.string(), error.what());
		Application::CallEvent(event);
		return false;
	}

	m_OnStateEntryFunc = CreateRef<sol::protected_function>((*m_SolEnvironment)["OnStateEntry"]);
	if (!m_OnStateEntryFunc->valid())
		m_OnStateEntryFunc.reset();

	m_OnStateUpdateFunc = CreateRef<sol::protected_function>((*m_SolEnvironment)["OnStateUpdate"]);
	if (!m_OnStateUpdateFunc->valid())
		m_OnStateUpdateFunc.reset();

	m_OnStateExitFunc = CreateRef<sol::protected_function>((*m_SolEnvironment)["OnStateExit"]);
	if (!m_OnStateExitFunc->valid())
		m_OnStateExitFunc.reset();

	lua.collect_garbage();
	return true;
}

template<typename... Args>
void LuaState::Call(const Ref<sol::protected_function>& function, const char* name, Args&&... args)
{
	if (!function)
		return;

	sol::protected_function_result result = function->call(std::forward<Args>(args)...);
	if (!result.valid())
	{
		sol::error error = result;
		ENGINE_ERROR("Failed to execute lua script '{0}' in state '{1}': {2}", name, GetName(), error.what());
		LuaErrorEvent luaErrorEvent(m_ScriptPath.string(), error.what());
		Application::CallEvent(luaErrorEvent);
	}
}

void LuaState::Enter()
{
	PROFILE_FUNCTION();
	Call(m_OnStateEntryFunc, "OnStateEntry");
}

void LuaState::During(float deltaTime)
{
	PROFILE_FUNCTION();
	Call(m_OnStateUpdateFunc, "OnStateUpdate", deltaTime);
}

void LuaState::Exit()
{
	PROFILE_FUNCTION();
	Call(m_OnStateExitFunc, "OnStateExit");
}

/* ------------------------------------------------------------------------------------------------------------------ */

void StateMachine::Bind(Entity entity)
{
	PROFILE_FUNCTION();

	for (const Ref<State>& state : m_States)
	{
		if (Ref<LuaState> luaState = std::dynamic_pointer_cast<LuaState>(state))
			luaState->Bind(entity, this);
	}

	m_CurrentState.reset();
	m_PreviousState.reset();
	m_PendingState.reset();
	m_Started = false;
}

void StateMachine::Update(float deltaTime)
{
	PROFILE_FUNCTION();

	if (!m_Started)
	{
		m_Started = true;
		if (Ref<State> initialState = GetState(m_InitialState))
			TransitionTo(initialState);
		else if (!m_States.empty())
			ENGINE_ERROR("State machine has no initial state named '{0}'", m_InitialState);
	}

	if (!m_CurrentState)
		return;

	m_InCallback = true;
	m_CurrentState->During(deltaTime);
	m_InCallback = false;

	if (m_PendingState)
		TransitionTo(m_PendingState);
}

bool StateMachine::ChangeState(const std::string& name)
{
	Ref<State> state = GetState(name);
	if (!state)
	{
		ENGINE_ERROR("State machine has no state named '{0}'", name);
		return false;
	}

	if (m_InCallback)
		m_PendingState = state;
	else
		TransitionTo(state);
	return true;
}

void StateMachine::TransitionTo(Ref<State> newState)
{
	PROFILE_FUNCTION();

	// A change made before the first update replaces the initial state
	m_Started = true;

	for (int i = 0; i < c_MaxTransitionsPerChange; ++i)
	{
		m_PendingState.reset();
		if (!newState || newState == m_CurrentState)
			return;

		m_InCallback = true;
		if (m_CurrentState)
			m_CurrentState->Exit();
		if (m_PendingState)
		{
			ENGINE_WARN("ChangeState called from OnStateExit of '{0}' is ignored", m_CurrentState->GetName());
			m_PendingState.reset();
		}

		m_PreviousState = m_CurrentState;
		m_CurrentState = newState;
		m_CurrentState->Enter();
		m_InCallback = false;

		newState = m_PendingState;
	}

	m_PendingState.reset();
	ENGINE_ERROR("State machine changed state more than {0} times in a row, stopped in '{1}'", c_MaxTransitionsPerChange, GetCurrentStateName());
}

void StateMachine::AddState(Ref<State> state)
{
	if (!state)
		return;
	if (GetState(state->GetName()))
	{
		ENGINE_WARN("State machine already has a state named '{0}', duplicate ignored", state->GetName());
		return;
	}
	m_States.push_back(state);
}

Ref<State> StateMachine::GetState(const std::string& name) const
{
	for (const Ref<State>& state : m_States)
		if (state->GetName() == name)
			return state;
	return nullptr;
}
