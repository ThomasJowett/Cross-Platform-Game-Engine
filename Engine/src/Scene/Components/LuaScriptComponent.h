#pragma once

#include "cereal/cereal.hpp"
#include <vector>
#include <filesystem>

struct LuaScriptComponent
{
	LuaScriptComponent() = default;
	LuaScriptComponent(const std::filesystem::path& filepath) : filepath(filepath) { }
	LuaScriptComponent(const LuaScriptComponent&) = default;

	std::filesystem::path filepath;
	
private:

	friend cereal::access;

	template<typename Archive>
	void save(Archive& archive) const
	{
		std::string relativePath;
		if (!filepath.empty())
			relativePath = FileUtils::RelativePath(filepath, Application::GetOpenDocumentDirectory()).string();
		archive(cereal::make_nvp("Script", relativePath));
	}

	template<typename Archive>
	void load(Archive& archive)
	{
		std::string relativePath;
		archive(cereal::make_nvp("Script", relativePath));
		if (!relativePath.empty())
			filepath = std::filesystem::absolute(Application::GetOpenDocumentDirectory() / relativePath);
	}

	std::function<void(void)> m_OnCreateFunc;
	std::function<void(void)> m_OnDestroyFunc;
	std::function<void(float deltaTime)> m_OnUpdateFunc;
	std::function<void(void)> m_OnFixedUpdateFunc;
};