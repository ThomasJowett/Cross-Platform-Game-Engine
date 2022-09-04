#include "stdafx.h"
#include "LuaScriptComponent.h"
#include "Scripting/Lua/LuaManager.h"
#include "Scene/SceneManager.h"
#include "Scene/Entity.h"
#include "box2d/box2d.h"

LuaScriptComponent::~LuaScriptComponent()
{
	if (m_SolEnvironment && LuaManager::IsValid())
	{
		OnDestroy();
	}
}

std::optional<std::pair<int, std::string>> LuaScriptComponent::ParseScript(Entity entity)
{
	LuaManager::GetState().collect_garbage();
	if (absoluteFilepath.empty())
	{
		return std::make_pair(0, "No file loaded");
	}

	if (!std::filesystem::exists(absoluteFilepath))
	{
		return std::make_pair(0, "File does not exist");
	}

	m_SolEnvironment = CreateRef<sol::environment>(LuaManager::GetState(), sol::create, LuaManager::GetState().globals());

	sol::protected_function_result result = LuaManager::GetState().script_file(absoluteFilepath.string(), *m_SolEnvironment, sol::script_pass_on_error);

	if (!result.valid())
	{
		sol::error error = result;

		std::string errorStr = error.what();
		int line = 1;
		auto linepos = errorStr.find(".lua:");
		std::string errorLine = errorStr.substr(linepos + 5); //+4 .lua: + 1
		auto lineposEnd = errorLine.find(":");
		errorLine = errorLine.substr(0, lineposEnd);
		line = std::stoi(errorLine);
		errorStr = errorStr.substr(linepos + errorLine.size() + lineposEnd + 4); //+4 .lua:

		return std::make_pair(line, errorStr);
	}

	(*m_SolEnvironment)["CurrentScene"] = SceneManager::CurrentScene();
	(*m_SolEnvironment)["CurrentEntity"] = entity;

	m_OnCreateFunc = CreateRef<sol::protected_function>((*m_SolEnvironment)["OnCreate"]);
	if (!m_OnCreateFunc->valid())
		m_OnCreateFunc.reset();

	m_OnDestroyFunc = CreateRef<sol::protected_function>((*m_SolEnvironment)["OnDestroy"]);
	if (!m_OnDestroyFunc->valid())
		m_OnDestroyFunc.reset();

	m_OnUpdateFunc = CreateRef<sol::protected_function>((*m_SolEnvironment)["OnUpdate"]);
	if (!m_OnUpdateFunc->valid())
		m_OnUpdateFunc.reset();

	m_OnFixedUpdateFunc = CreateRef<sol::protected_function>((*m_SolEnvironment)["OnFixedUpdate"]);
	if (!m_OnFixedUpdateFunc->valid())
		m_OnFixedUpdateFunc.reset();

	m_OnDebugRenderFunc = CreateRef<sol::protected_function>((*m_SolEnvironment)["OnDebugRender"]);
	if (!m_OnDebugRenderFunc->valid())
		m_OnDebugRenderFunc.reset();

	m_OnBeginContactFunc = CreateRef<sol::protected_function>((*m_SolEnvironment)["OnBeginContact"]);
	if (!m_OnBeginContactFunc->valid())
		m_OnBeginContactFunc.reset();

	m_OnEndContactFunc = CreateRef<sol::protected_function>((*m_SolEnvironment)["OnEndContact"]);
	if (!m_OnEndContactFunc->valid())
		m_OnEndContactFunc.reset();

	LuaManager::GetState().collect_garbage();
	return std::nullopt;
}

void LuaScriptComponent::OnCreate()
{
	PROFILE_FUNCTION();
	if (m_OnCreateFunc)
	{
		sol::protected_function_result result = m_OnCreateFunc->call();
		if (!result.valid())
		{
			sol::error error = result;
			CLIENT_ERROR("Failed to execute lua script 'OnCreate': {0}", error.what());
		}
	}
}

void LuaScriptComponent::OnDestroy()
{
	PROFILE_FUNCTION();
	if (m_OnDestroyFunc)
	{
		sol::protected_function_result result = m_OnDestroyFunc->call();
		if (!result.valid())
		{
			sol::error error = result;
			CLIENT_ERROR("Failed to execute lua script 'OnDestroy': {0}", error.what());
		}
	}
}

void LuaScriptComponent::OnUpdate(float deltaTime)
{
	PROFILE_FUNCTION();
	if (m_OnUpdateFunc)
	{
		sol::protected_function_result result = m_OnUpdateFunc->call(deltaTime);
		if (!result.valid())
		{
			sol::error error = result;
			CLIENT_ERROR("Failed to execute lua script 'OnUpdate': {0}", error.what());
		}
	}
}

void LuaScriptComponent::OnFixedUpdate()
{
	PROFILE_FUNCTION();
	if (m_OnFixedUpdateFunc)
	{
		sol::protected_function_result result = m_OnFixedUpdateFunc->call();
		if (!result.valid())
		{
			sol::error error = result;
			CLIENT_ERROR("Failed to execute lua script 'OnFixedUpdate': {0}", error.what());
		}
	}
}

void LuaScriptComponent::OnDebugRender()
{
	PROFILE_FUNCTION();
	if (m_OnDebugRenderFunc)
	{
		sol::protected_function_result result = m_OnDebugRenderFunc->call();
		if (!result.valid())
		{
			sol::error error = result;
			CLIENT_ERROR("Failed to execute lua script 'OnDebugRender': {0}", error.what());
		}
	}
}

void LuaScriptComponent::OnBeginContact(b2Fixture* fixture)
{
	PROFILE_FUNCTION();
	ASSERT(std::find(m_Fixtures.begin(), m_Fixtures.end(), fixture) == m_Fixtures.end(), "Should not have a begin contact event for own fixtures");

	if (m_OnBeginContactFunc)
	{
		Entity entity((entt::entity)fixture->GetUserData().pointer, SceneManager::CurrentScene());
		sol::protected_function_result result = m_OnBeginContactFunc->call(entity);
		if (!result.valid())
		{
			sol::error error = result;
			CLIENT_ERROR("Failed to execute lua script 'OnBeginContact': {0}", error.what());
		}
	}
}

void LuaScriptComponent::OnEndContact(b2Fixture* fixture)
{
	PROFILE_FUNCTION();
	ASSERT(std::find(m_Fixtures.begin(), m_Fixtures.end(), fixture) == m_Fixtures.end(), "Should not have an end contact event for own fixtures");

	if (m_OnEndContactFunc)
	{
		Entity entity((entt::entity)fixture->GetUserData().pointer, SceneManager::CurrentScene());
		sol::protected_function_result result = m_OnEndContactFunc->call(entity);
		if (!result.valid())
		{
			sol::error error = result;
			CLIENT_ERROR("Failed to execute lua script 'OnEndContact': {0}", error.what());
		}
	}
}

bool LuaScriptComponent::IsContactListener()
{
	return m_OnBeginContactFunc || m_OnEndContactFunc;
}