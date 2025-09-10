#include "Tasks.h"

#include "Scripting/Lua/LuaManager.h"

#include "Logging/Instrumentor.h"
#include "Scene/SceneManager.h"
#include "Scripting/Lua/LuaErrorEvent.h"

#include "sol/sol.hpp"

BehaviourTree::CustomTask::CustomTask(BehaviourTree* behaviourTree, const std::filesystem::path& filepath)
	:Leaf(behaviourTree)
{
	PROFILE_FUNCTION();

	m_LuaScript = CreateRef<LuaScript>(filepath);

	if (!m_LuaScript)
	{
		ENGINE_ERROR("could not find custom task lua script");
	}

	m_SolEnvironment = CreateRef<sol::environment>(LuaManager::GetState(), sol::create, LuaManager::GetState().globals());

	sol::protected_function_result result = LuaManager::GetState().script(m_LuaScript->GetSource(), *m_SolEnvironment, sol::script_pass_on_error);

	if (!result.valid())
	{
		sol::error error = result;

		auto event = LuaErrorEvent(filepath.string(), error.what());
		Application::CallEvent(event);
	}

	(*m_SolEnvironment)["CurrentScene"] = SceneManager::CurrentScene();

	m_OnStateEntryFunc = CreateRef<sol::protected_function>((*m_SolEnvironment)["OnStateEntry"]);
	if (!m_OnStateEntryFunc->valid())
		m_OnStateEntryFunc.reset();

	m_OnStateUpdateFunc = CreateRef<sol::protected_function>((*m_SolEnvironment)["OnStateUpdate"]);
	if (!m_OnStateUpdateFunc->valid())
		m_OnStateUpdateFunc.reset();

	m_OnStateExitFunc = CreateRef<sol::protected_function>((*m_SolEnvironment)["OnStateExit"]);
	if (!m_OnStateExitFunc->valid())
		m_OnStateExitFunc.reset();

	LuaManager::GetState().collect_garbage();
}

BehaviourTree::CustomTask::~CustomTask()
{
}

void BehaviourTree::CustomTask::initialize()
{
	PROFILE_FUNCTION();
	if (m_OnStateEntryFunc)
	{
		sol::protected_function_result result = m_OnStateEntryFunc->call();
		if (!result.valid())
		{
			sol::error error = result;
			ENGINE_ERROR("Failed to execute lua script 'OnStateEntry': {0}", error.what());
			LuaErrorEvent luaErrorEvent(m_LuaScript->GetFilepath().string(), error.what());
			Application::CallEvent(luaErrorEvent);
		}
	}
}

BehaviourTree::Node::Status BehaviourTree::CustomTask::update(float deltaTime)
{
	PROFILE_FUNCTION();
	if (m_OnStateUpdateFunc)
	{
		sol::protected_function_result result = m_OnStateUpdateFunc->call(deltaTime);
		auto type = result.get_type();
		if (!result.valid())
		{
			sol::error error = result;
			ENGINE_ERROR("Failed to execute lua script 'OnStateUpdate': {0}", error.what());
			LuaErrorEvent luaErrorEvent(m_LuaScript->GetFilepath().string(), error.what());
			Application::CallEvent(luaErrorEvent);
			return Status::Invalid;
		}
		if (result.get_type() == sol::type::number) {
			return result.get<Status>();
		}
		else {
			ENGINE_ERROR("Lua function 'OnStateUpdate' returned an unexpected type, expected NodeStatus");
			return Status::Invalid;
		}
	}
	return Status::Success;
}

void BehaviourTree::CustomTask::terminate(Status s)
{
	PROFILE_FUNCTION();
	if (m_OnStateExitFunc)
	{
		sol::protected_function_result result = m_OnStateExitFunc->call(s);
		if (!result.valid())
		{
			sol::error error = result;
			ENGINE_ERROR("Failed to execute lua script 'OnStateExit': {0}", error.what());
			LuaErrorEvent luaErrorEvent(m_LuaScript->GetFilepath().string(), error.what());
			Application::CallEvent(luaErrorEvent);
		}
	}
}
