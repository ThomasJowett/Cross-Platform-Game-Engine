#pragma once

#include "BehaviorTree.h"

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
			m_BehaviourTree->setEntry(nullptr);
			return Node::Status::Success;
		}
		else
		{
			return Node::Status::Running;
			m_BehaviourTree->setEntry(this);
		}
	}

	float getWaitTime() const { return m_WaitTime; }

private:
	float m_WaitTime;
	float m_CurrentTime;
};
}
