#pragma once

#include "BehaviourTree.h"
#include "sol/sol.hpp"
#include "Asset/LuaScript.h"

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
}
