#include "stdafx.h"
#include "StateMachine.h"

void StateMachine::Update()
{
	if (m_CurrentState) 
		m_CurrentState->During();
}

void StateMachine::ChangeState(Ref<State> newState)
{
	if (newState != m_CurrentState)
	{
		m_PreviousState = m_CurrentState;
		if (m_CurrentState)
			m_CurrentState->Exit();
		m_CurrentState = newState;
		if (m_CurrentState)
			m_CurrentState->Enter();
	}
}
