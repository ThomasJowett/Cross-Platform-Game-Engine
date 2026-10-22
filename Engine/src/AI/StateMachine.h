#pragma once

#include "Core/core.h"
#include "math/Vector2f.h"
#include "Asset/LuaScript.h"
#include "Scene/Entity.h"

#include "sol/sol.hpp"

#include <filesystem>
#include <string>
#include <vector>

class StateMachine;

class State
{
public:
	explicit State(const std::string& name) : m_Name(name) {}
	virtual ~State() = default;

	virtual void Enter() {}
	virtual void During(float deltaTime) {}
	virtual void Exit() {}

	const std::string& GetName() const { return m_Name; }

	Vector2f GetEditorPosition() const { return m_EditorPosition; }
	void SetEditorPosition(Vector2f editorPosition) { m_EditorPosition = editorPosition; }

private:
	std::string m_Name;
	Vector2f m_EditorPosition;
};

// Runs a Lua script's OnStateEntry, OnStateUpdate and OnStateExit, the same way a behaviour tree CustomTask does
class LuaState : public State
{
public:
	LuaState(const std::string& name, const std::filesystem::path& filepath);
	~LuaState();

	void Enter() override;
	void During(float deltaTime) override;
	void Exit() override;

	// Runs the script in a fresh environment with CurrentScene, CurrentEntity and StateMachine set
	bool Bind(Entity entity, StateMachine* stateMachine);

	const std::filesystem::path& GetScriptPath() const { return m_ScriptPath; }

private:
	template<typename... Args>
	void Call(const Ref<sol::protected_function>& function, const char* name, Args&&... args);

	std::filesystem::path m_ScriptPath;
	Ref<LuaScript> m_LuaScript;

	Ref<sol::environment> m_SolEnvironment;
	Ref<sol::protected_function> m_OnStateEntryFunc;
	Ref<sol::protected_function> m_OnStateUpdateFunc;
	Ref<sol::protected_function> m_OnStateExitFunc;
};

class StateMachine : public std::enable_shared_from_this<StateMachine>
{
public:
	struct Transition
	{
		std::string from;
		std::string to;
	};

	// Binds every Lua state to the entity and resets, so the initial state is entered on the next update
	void Bind(Entity entity);
	void Update(float deltaTime);

	// Called from inside a state's callbacks, the change happens once that callback returns
	bool ChangeState(const std::string& name);

	void AddState(Ref<State> state);
	Ref<State> GetState(const std::string& name) const;
	const std::vector<Ref<State>>& GetStates() const { return m_States; }

	const std::string& GetInitialState() const { return m_InitialState; }
	void SetInitialState(const std::string& name) { m_InitialState = name; }

	// Editor only, the arrows drawn between states; transitions are made by calling ChangeState
	void AddTransition(const std::string& from, const std::string& to) { m_Transitions.push_back({ from, to }); }
	const std::vector<Transition>& GetTransitions() const { return m_Transitions; }

	std::string GetCurrentStateName() const { return m_CurrentState ? m_CurrentState->GetName() : std::string(); }
	std::string GetPreviousStateName() const { return m_PreviousState ? m_PreviousState->GetName() : std::string(); }

	Vector2f GetEditorPosition() const { return m_EditorPosition; }
	void SetEditorPosition(Vector2f editorPosition) { m_EditorPosition = editorPosition; }

private:
	void TransitionTo(Ref<State> newState);

	std::vector<Ref<State>> m_States;
	std::vector<Transition> m_Transitions;
	std::string m_InitialState;

	Ref<State> m_CurrentState;
	Ref<State> m_PreviousState;
	Ref<State> m_PendingState;
	bool m_Started = false;
	bool m_InCallback = false;

	Vector2f m_EditorPosition; // Entry node
};
