#include "stdafx.h"
#include "LuaScript.h"
#include "Core/Application.h"

LuaScript::LuaScript(const std::filesystem::path& filepath)
{
    Load(filepath);
}

LuaScript::LuaScript(const std::filesystem::path& filepath, const std::vector<uint8_t>& data)
{
	Load(filepath, data);
}

bool LuaScript::Load(const std::filesystem::path& filepath)
{
	std::filesystem::path absolutePath = std::filesystem::absolute(Application::GetOpenDocumentDirectory() / filepath);
	std::ifstream file(absolutePath);
	if (!file.is_open()) {
		ENGINE_ERROR("Failed to open Lua script file: {0}", absolutePath.string());
		return false;
	}

	std::stringstream buffer;
	buffer << file.rdbuf();
	m_Source = buffer.str();

	m_Filepath = filepath;
	m_Filepath.make_preferred();
	return true;
}

bool LuaScript::Load(const std::filesystem::path& filepath, const std::vector<uint8_t>& data)
{
	m_Source = std::string(data.begin(), data.end());
	m_Filepath = filepath;
	m_Filepath.make_preferred();
	return true;
}
