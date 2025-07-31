#include "stdafx.h"
#include "LuaScriptComponent.h"
#include "Scripting/Lua/LuaManager.h"
#include "Scene/SceneManager.h"
#include "Scene/Entity.h"
#include "Scene/AssetManager.h"
#include "box2d/box2d.h"
#include "Logging/Instrumentor.h"

LuaScriptComponent::LuaScriptComponent(const std::filesystem::path& filepath)
{
	script = AssetManager::GetAsset<LuaScript>(filepath);
}

LuaScriptComponent::~LuaScriptComponent()
{
	if (m_SolEnvironment && LuaManager::IsValid())
	{
		OnDestroy();
	}
}

bool LuaScriptComponent::ParseScript(Entity entity)
{
	PROFILE_FUNCTION();

	LuaManager::GetState().collect_garbage();
	if (!script)
	{
		return false;
	}

	m_SolEnvironment = CreateRef<sol::environment>(LuaManager::GetState(), sol::create, LuaManager::GetState().globals());

	sol::protected_function_result result = LuaManager::GetState().script(script->GetSource(), *m_SolEnvironment, sol::script_pass_on_error);

	if (!result.valid())
	{
		sol::error error = result;

		auto [line, file, errorStr] = ParseLuaError(error.what());

		auto event = LuaErrorEvent(line, file, errorStr);
		Application::CallEvent(event);

		return false;
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
	return true;
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

			auto [line, file, errorStr] = ParseLuaError(error.what());
			LuaErrorEvent luaErrorEvent(line, file, errorStr);
			Application::CallEvent(luaErrorEvent);
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
			auto [line, file, errorStr] = ParseLuaError(error.what());
			LuaErrorEvent luaErrorEvent(line, file, errorStr);
			Application::CallEvent(luaErrorEvent);
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

			auto [line, file, errorStr] = ParseLuaError(error.what());
			LuaErrorEvent luaErrorEvent(line, file, errorStr);
			Application::CallEvent(luaErrorEvent);
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

			auto [line, file, errorStr] = ParseLuaError(error.what());
			LuaErrorEvent luaErrorEvent(line, file, errorStr);
			Application::CallEvent(luaErrorEvent);
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

			auto [line, file, errorStr] = ParseLuaError(error.what());
			LuaErrorEvent luaErrorEvent(line, file, errorStr);
			Application::CallEvent(luaErrorEvent);
		}
	}
}

void LuaScriptComponent::OnBeginContact(b2Fixture* fixture, Vector2f normal, Vector2f point)
{
	PROFILE_FUNCTION();
	ASSERT(std::find(m_Fixtures.begin(), m_Fixtures.end(), fixture) == m_Fixtures.end(), "Should not have a begin contact event for own fixtures");

	if (m_OnBeginContactFunc)
	{
		Entity entity((entt::entity)fixture->GetUserData().pointer, SceneManager::CurrentScene());
		sol::protected_function_result result = m_OnBeginContactFunc->call(entity, normal, point);
		if (!result.valid())
		{
			sol::error error = result;
			CLIENT_ERROR("Failed to execute lua script 'OnBeginContact': {0}", error.what());

			auto [line, file, errorStr] = ParseLuaError(error.what());
			LuaErrorEvent luaErrorEvent(line, file, errorStr);
			Application::CallEvent(luaErrorEvent);
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

			auto [line, file, errorStr] = ParseLuaError(error.what());
			LuaErrorEvent luaErrorEvent(line, file, errorStr);
			Application::CallEvent(luaErrorEvent);
		}
	}
}

bool LuaScriptComponent::IsContactListener()
{
	return m_OnBeginContactFunc || m_OnEndContactFunc;
}

std::tuple<int, std::string, std::string> LuaScriptComponent::ParseLuaError(const std::string& errorMessage)
{
	PROFILE_FUNCTION();
	std::string errorStr = errorMessage;
	int line = 1;
	size_t chunkStart = errorStr.find("[string ");
	if (chunkStart != std::string::npos) {
		// Example: [string "chunk"]:3:
		size_t closeQuote = errorStr.find("\"]:", chunkStart);
		if (closeQuote != std::string::npos) {
			size_t lineNumStart = closeQuote + 3;
			size_t lineNumEnd = errorStr.find(':', lineNumStart);
			if (lineNumEnd != std::string::npos) {
				std::string lineStr = errorStr.substr(lineNumStart, lineNumEnd - lineNumStart);
				try {
					line = std::stoi(lineStr);
				}
				catch (...) {
					line = 1;
				}
				errorStr = errorStr.substr(lineNumEnd + 1);
			}
		}
	}
	return std::make_tuple(line, script->GetFilepath().string(), errorStr);
}
