#pragma once

#include "BehaviorTree.h"

//Wait for the specified time when executed
class Wait : public BehaviourTree::Leaf
{
public:
	explicit Wait(float waitTime)
		:m_WaitTime(waitTime), m_CurrentTime(0.0f)
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

private:
	float m_WaitTime;
	float m_CurrentTime;
};