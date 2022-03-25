#pragma once

#include "cereal/cereal.hpp"
#include "sol/sol.hpp"
#include <vector>
#include <filesystem>

#include "Core/Application.h"
#include "Utilities/FileUtils.h"

struct LuaScriptComponent
{
	LuaScriptComponent() = default;
	LuaScriptComponent(const std::filesystem::path& filepath) : absoluteFilepath(filepath) { }
	LuaScriptComponent(const LuaScriptComponent&) = default;

	~LuaScriptComponent();

	std::filesystem::path absoluteFilepath;
	bool created = false;

	bool ParseScript();

	void OnCreate();
	void OnDestroy();
	void OnUpdate(float deltaTime);
	void OnFixedUpdate();

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

	Ref<sol::environment> m_SolEnvironment;
	Ref<sol::protected_function> m_OnCreateFunc;
	Ref<sol::protected_function> m_OnDestroyFunc;
	Ref<sol::protected_function> m_OnUpdateFunc;
	Ref<sol::protected_function> m_OnFixedUpdateFunc;
};