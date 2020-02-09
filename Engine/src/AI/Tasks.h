#pragma once

#include "behaviorTree.h"

//Wait for the specified time when executed
class Wait : public BehaviorTree::Leaf
{
public:
	Wait(float waitTime)
		:mWaitTime(waitTime)
	{
		m_CurrentTime = 0.0f;
	}

	void initialize()
	{
		m_CurrentTime = mWaitTime;
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
	float mWaitTime;
	float m_CurrentTime;
};