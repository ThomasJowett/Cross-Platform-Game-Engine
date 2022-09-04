#pragma once

#include "cereal/cereal.hpp"
#include "sol/sol.hpp"
#include <vector>
#include <filesystem>

#include "Core/Application.h"
#include "Utilities/FileUtils.h"

class Entity;
struct RigidBody2DComponent;
class b2Fixture;

struct LuaScriptComponent
{
	LuaScriptComponent() = default;
	LuaScriptComponent(const std::filesystem::path& filepath) : absoluteFilepath(filepath) { }
	LuaScriptComponent(const LuaScriptComponent&) = default;

	~LuaScriptComponent();

	std::filesystem::path absoluteFilepath;
	bool created = false;

	std::optional<std::pair<int, std::string>> ParseScript(Entity entity);

	void OnCreate();
	void OnDestroy();
	void OnUpdate(float deltaTime);
	void OnFixedUpdate();
	void OnDebugRender();
	void OnBeginContact(b2Fixture* fixture);
	void OnEndContact(b2Fixture* fixture);
	bool IsContactListener();

	const std::vector<b2Fixture*>& GetFixtures() const { return m_Fixtures; }
	//const std::vector<b2Fixture*>& GetContacts() const { return m_Contacts; }

private:
	friend cereal::access;

	template<typename Archive>
	void save(Archive& archive) const
	{
		std::string relativePath;
		if (!absoluteFilepath.empty())
			relativePath = FileUtils::RelativePath(absoluteFilepath, Application::GetOpenDocumentDirectory()).string();
		archive(cereal::make_nvp("Script", relativePath));
	}

	template<typename Archive>
	void load(Archive& archive)
	{
		std::string relativePath;
		archive(cereal::make_nvp("Script", relativePath));
		if (!relativePath.empty())
			absoluteFilepath = std::filesystem::absolute(Application::GetOpenDocumentDirectory() / relativePath);
	}

	friend RigidBody2DComponent;

	std::vector<b2Fixture*> m_Fixtures;

	Ref<sol::environment> m_SolEnvironment;
	Ref<sol::protected_function> m_OnCreateFunc;
	Ref<sol::protected_function> m_OnDestroyFunc;
	Ref<sol::protected_function> m_OnUpdateFunc;
	Ref<sol::protected_function> m_OnFixedUpdateFunc;
	Ref<sol::protected_function> m_OnDebugRenderFunc;
	Ref<sol::protected_function> m_OnBeginContactFunc;
	Ref<sol::protected_function> m_OnEndContactFunc;
};