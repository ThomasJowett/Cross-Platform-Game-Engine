#pragma once

#include "BehaviourTree.h"
#include "sol/sol.hpp"
#include "Asset/LuaScript.h"
#include "Scene/Entity.h"

namespace BehaviourTree
{

//Wait for the specified time when executed
class Wait : public Leaf
{
public:
	explicit Wait(BehaviourTree* behaviourTree, float waitTime)
		:Leaf(behaviourTree), m_WaitTime(waitTime), m_CurrentTime(0.0f)
	{
	}

	void initialize() final
	{
		m_CurrentTime = m_WaitTime;
	}

	Status update(float deltaTime) override
	{
		m_CurrentTime -= deltaTime;

		if (m_CurrentTime <= 0.0f)
		{
			m_CurrentTime = 0.0f;
			return Node::Status::Success;
		}
		else
		{
			return Node::Status::Running;
		}
	}

	float getWaitTime() const { return m_WaitTime; }

private:
	float m_WaitTime;
	float m_CurrentTime;
};

class CustomTask : public Leaf
{
public:
	CustomTask(BehaviourTree* behaviourTree, const std::filesystem::path& filepath);
	~CustomTask();

	void initialize() final;
	Status update(float deltaTime) final;
	void terminate(Status s) final;

	// Runs the script in a fresh environment with CurrentScene, CurrentEntity and Blackboard set
	bool Bind(Entity entity, Ref<Blackboard> blackboard);

	Ref<LuaScript> getLuaScript() const { return m_LuaScript; }
	const std::filesystem::path& getScriptPath() const { return m_ScriptPath; }

private:
	std::filesystem::path m_ScriptPath;
	Ref<LuaScript> m_LuaScript;

	Ref<sol::environment> m_SolEnvironment;
	Ref<sol::protected_function> m_OnStateEntryFunc;
	Ref<sol::protected_function> m_OnStateUpdateFunc;
	Ref<sol::protected_function> m_OnStateExitFunc;
};

//--------------------------------------------------------------------------------------------------------------------

// Waits for a random time between the min and max each time it starts
class RandomWait : public Leaf
{
public:
	RandomWait(BehaviourTree* behaviourTree, float minTime, float maxTime)
		:Leaf(behaviourTree), m_MinTime(minTime), m_MaxTime(maxTime)
	{
	}

	void initialize() final;
	Status update(float deltaTime) override;

	float getMinTime() const { return m_MinTime; }
	float getMaxTime() const { return m_MaxTime; }

private:
	float m_MinTime;
	float m_MaxTime;
	float m_CurrentTime = 0.0f;
};

//--------------------------------------------------------------------------------------------------------------------

// Sets a blackboard key to a fixed value, then succeeds
class SetBlackboard : public Leaf
{
public:
	enum class ValueType
	{
		Bool,
		Int,
		Float,
		Double,
		String,
		Vec2,
		Vec3
	};

	struct Value
	{
		ValueType type = ValueType::Bool;

		// Only the value matching the type is used
		bool boolValue = false;
		int intValue = 0;
		double numberValue = 0.0; // Float and Double
		std::string stringValue;
		Vector3f vectorValue; // Vec2 uses x and y
	};

	SetBlackboard(BehaviourTree* behaviourTree, Ref<Blackboard> blackboard, const std::string& key, const Value& value)
		:Leaf(behaviourTree), m_Blackboard(blackboard), m_Key(key), m_Value(value)
	{
	}

	Status update(float deltaTime) override;

	const std::string& getKey() const { return m_Key; }
	const Value& getValue() const { return m_Value; }

private:
	Ref<Blackboard> m_Blackboard;
	std::string m_Key;
	Value m_Value;
};

//--------------------------------------------------------------------------------------------------------------------

// Emits a signal on the Lua signal bus with the owning entity as the sender, then succeeds
class EmitSignal : public Leaf
{
public:
	EmitSignal(BehaviourTree* behaviourTree, const std::string& signalName)
		:Leaf(behaviourTree), m_SignalName(signalName)
	{
	}

	void Bind(Entity entity) { m_Entity = entity; }
	Status update(float deltaTime) override;

	const std::string& getSignalName() const { return m_SignalName; }

private:
	std::string m_SignalName;
	Entity m_Entity;
};
}
