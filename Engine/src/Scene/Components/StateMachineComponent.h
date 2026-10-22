#pragma once

#include "cereal/access.hpp"

#include "AI/StateMachine.h"
#include "AI/StateMachineSerializer.h"
#include "Scripting/Lua/LuaBindings.h"

struct StateMachineComponent
{
	StateMachineComponent() = default;
	// Copies reload the state machine so each entity gets its own current state and Lua environments
	StateMachineComponent(const StateMachineComponent& other)
		:filepath(other.filepath)
	{
		stateMachine = filepath.empty() ? other.stateMachine : StateMachineSerializer::Load(filepath);
	}
	StateMachineComponent& operator=(const StateMachineComponent& other)
	{
		if (this != &other)
		{
			StateMachineComponent copy(other);
			*this = std::move(copy);
		}
		return *this;
	}
	StateMachineComponent(StateMachineComponent&&) = default;
	StateMachineComponent& operator=(StateMachineComponent&&) = default;

	// Loaded per entity rather than shared through AssetManager, as the state machine holds runtime state
	Ref<StateMachine> stateMachine;

	std::filesystem::path filepath;

	std::string GetCurrentStateName() const { return stateMachine ? stateMachine->GetCurrentStateName() : std::string(); }
	std::string GetPreviousStateName() const { return stateMachine ? stateMachine->GetPreviousStateName() : std::string(); }
	bool ChangeState(const std::string& name) { return stateMachine && stateMachine->ChangeState(name); }

	REFLECT_LUA_BEGIN(StateMachineComponent)
		REFLECT_LUA_FUNCTION(GetCurrentStateName, "Get the name of the active state, or an empty string before the first update")
		REFLECT_LUA_FUNCTION(GetPreviousStateName, "Get the name of the state that was active before the current one, or an empty string")
		REFLECT_LUA_FUNCTION(ChangeState, "Change to the named state, running OnStateExit on the old state and OnStateEntry on the new one; returns false if there's no state with that name")
	REFLECT_LUA_END()

private:
	friend cereal::access;
	template<typename Archive>
	void save(Archive& archive) const
	{
		archive(filepath.string());
	}

	template<typename Archive>
	void load(Archive& archive)
	{
		std::string relativePath;
		archive(relativePath);

		if (!relativePath.empty())
		{
			filepath = relativePath;
			stateMachine = StateMachineSerializer::Load(filepath);
		}
	}
};
