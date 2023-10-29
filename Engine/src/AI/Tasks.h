#pragma once

#include "BehaviourTree.h"
#include "sol/sol.hpp"

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

	void terminate(Node::Status status) final
	{
		ENGINE_DEBUG("Waited: {0}", m_WaitTime);
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

	const std::filesystem::path& getFilePath() { return m_AbsoluteFilepath; }

private:
	std::filesystem::path m_AbsoluteFilepath;

	Ref<sol::environment> m_SolEnvironment;
	Ref<sol::protected_function> m_OnStateEntryFunc;
	Ref<sol::protected_function> m_OnStateUpdateFunc;
	Ref<sol::protected_function> m_OnStateExitFunc;
};
}
