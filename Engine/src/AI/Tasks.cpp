#include "Tasks.h"

#include "Scripting/Lua/LuaManager.h"

#include "Logging/Instrumentor.h"
#include "Scene/SceneManager.h"
#include "Scene/AssetManager.h"
#include "Scene/Entity.h"
#include "Scripting/Lua/LuaErrorEvent.h"
#include "Utilities/Random.h"

#include "sol/sol.hpp"

#include <functional>

BehaviourTree::CustomTask::CustomTask(BehaviourTree* behaviourTree, const std::filesystem::path& filepath)
	:Leaf(behaviourTree), m_ScriptPath(filepath)
{
	PROFILE_FUNCTION();

	if (!filepath.empty())
		m_LuaScript = AssetManager::GetAsset<LuaScript>(filepath);
}

bool BehaviourTree::CustomTask::Bind(Entity entity, Ref<Blackboard> blackboard)
{
	PROFILE_FUNCTION();

	m_SolEnvironment.reset();
	m_OnStateEntryFunc.reset();
	m_OnStateUpdateFunc.reset();
	m_OnStateExitFunc.reset();

	if (!m_LuaScript)
	{
		ENGINE_ERROR("Custom task has no lua script");
		return false;
	}

	m_SolEnvironment = CreateRef<sol::environment>(LuaManager::GetState(), sol::create, LuaManager::GetState().globals());

	(*m_SolEnvironment)["CurrentScene"] = SceneManager::CurrentScene();
	(*m_SolEnvironment)["CurrentEntity"] = entity;
	(*m_SolEnvironment)["Blackboard"] = blackboard;

	sol::protected_function_result result = LuaManager::GetState().script(m_LuaScript->GetSource(), *m_SolEnvironment, sol::script_pass_on_error);

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

	LuaManager::GetState().collect_garbage();
	return true;
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
			LuaErrorEvent luaErrorEvent(m_ScriptPath.string(), error.what());
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
			LuaErrorEvent luaErrorEvent(m_ScriptPath.string(), error.what());
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
			LuaErrorEvent luaErrorEvent(m_ScriptPath.string(), error.what());
			Application::CallEvent(luaErrorEvent);
		}
	}
}

void BehaviourTree::BehaviourTree::Bind(Entity entity)
{
	PROFILE_FUNCTION();

	std::function<void(const Ref<Node>&)> bindNode = [&](const Ref<Node>& node)
		{
			if (!node)
				return;

			if (Ref<CustomTask> task = std::dynamic_pointer_cast<CustomTask>(node))
				task->Bind(entity, m_Blackboard);
			else if (Ref<EmitSignal> emitSignal = std::dynamic_pointer_cast<EmitSignal>(node))
				emitSignal->Bind(entity);
			else if (Ref<Composite> composite = std::dynamic_pointer_cast<Composite>(node))
				for (const Ref<Node>& child : *composite)
					bindNode(child);
			else if (Ref<Decorator> decorator = std::dynamic_pointer_cast<Decorator>(node))
				bindNode(decorator->getChild());
		};

	bindNode(m_Root);
}

void BehaviourTree::RandomWait::initialize()
{
	m_CurrentTime = Random::FloatInRange(std::min(m_MinTime, m_MaxTime), std::max(m_MinTime, m_MaxTime));
}

BehaviourTree::Node::Status BehaviourTree::RandomWait::update(float deltaTime)
{
	m_CurrentTime -= deltaTime;
	return m_CurrentTime <= 0.0f ? Status::Success : Status::Running;
}

BehaviourTree::Node::Status BehaviourTree::SetBlackboard::update(float deltaTime)
{
	switch (m_Value.type)
	{
	case ValueType::Bool:	m_Blackboard->setBool(m_Key, m_Value.boolValue); break;
	case ValueType::Int:	m_Blackboard->setInt(m_Key, m_Value.intValue); break;
	case ValueType::Float:	m_Blackboard->setFloat(m_Key, (float)m_Value.numberValue); break;
	case ValueType::Double:	m_Blackboard->setDouble(m_Key, m_Value.numberValue); break;
	case ValueType::String:	m_Blackboard->setString(m_Key, m_Value.stringValue); break;
	case ValueType::Vec2:	m_Blackboard->setVector2(m_Key, Vector2f(m_Value.vectorValue.x, m_Value.vectorValue.y)); break;
	case ValueType::Vec3:	m_Blackboard->setVector3(m_Key, m_Value.vectorValue); break;
	}
	return Status::Success;
}

BehaviourTree::Node::Status BehaviourTree::EmitSignal::update(float deltaTime)
{
	PROFILE_FUNCTION();

	if (!m_Entity || m_SignalName.empty())
		return Status::Failure;

	LuaManager::GetSignalBus().Emit(m_SignalName, m_Entity, LuaManager::GetState().create_table());
	return Status::Success;
}
