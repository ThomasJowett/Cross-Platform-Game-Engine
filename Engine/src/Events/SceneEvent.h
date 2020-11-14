#pragma once

#include "Event.h"
#include <filesystem>

class SceneChanged : public Event
{
public:
	SceneChanged(const std::filesystem::path& filepath)
		:m_Filepath(filepath)
	{}

	const std::filesystem::path& GetFilepath() const { return m_Filepath; }

	std::string to_string() const override
	{
		std::stringstream ss;
		ss << "Scene changed to: " << m_Filepath.string();
		return ss.str();
	}

	EVENT_CLASS_TYPE(SCENE_CHANGED);
	EVENT_CLASS_CATEGORY(EC_SCENE);
private:
	const std::filesystem::path m_Filepath;
};

class SceneSaved : public Event
{
public:
	SceneSaved(const std::filesystem::path& filepath)
		:m_Filepath(filepath)
	{}

	const std::filesystem::path& GetFilePath() const { return m_Filepath; }

	std::string to_string() const override
	{
		std::stringstream ss;
		ss << "Scene saved to: " << m_Filepath.string();
		return ss.str();
	}

	EVENT_CLASS_TYPE(SCENE_SAVED);
	EVENT_CLASS_CATEGORY(EC_SCENE);
private:
	const std::filesystem::path m_Filepath;
};

class SceneLoaded : public Event
{
public:
	SceneLoaded(const std::filesystem::path& filepath)
		:m_Filepath(filepath)
	{}

	const std::filesystem::path& GetFilePath() const { return m_Filepath; }

	std::string to_string() const override
	{
		std::stringstream ss;
		ss << "Scene loaded from : " << m_Filepath.string();
		return ss.str();
	}

	EVENT_CLASS_TYPE(SCENE_SAVED);
	EVENT_CLASS_CATEGORY(EC_SCENE);
private:
	const std::filesystem::path m_Filepath;
};