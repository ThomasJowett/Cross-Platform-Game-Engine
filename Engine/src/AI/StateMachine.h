#pragma once

#include "Core/core.h"

class State
{
public:
	virtual ~State() = default;
	virtual void Enter() = 0;
	virtual void During() = 0;
	virtual void Exit() = 0;
};

class StateMachine
{
public:
	void Update();
	void ChangeState(Ref<State> newState);
private:
	Ref<State> m_CurrentState;
	Ref<State> m_PreviousState;
};
