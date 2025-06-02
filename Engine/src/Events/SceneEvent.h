#pragma once

#include "Event.h"
#include <filesystem>

enum class SceneState;

class SceneChangedEvent : public Event
{
public:
	SceneChangedEvent(const std::filesystem::path& filepath)
		:m_Filepath(filepath)
	{}

	std::filesystem::path GetFilepath() const { return m_Filepath; }

	std::string to_string() const override
	{
		std::stringstream ss;
		ss << "Scene changed to: " << m_Filepath.string();
		return ss.str();
	}

	EVENT_CLASS_TYPE(SCENE_CHANGED);
	EVENT_CLASS_CATEGORY(EventCategory::SCENE);
private:
	const std::filesystem::path m_Filepath;
};

/* ------------------------------------------------------------------------------------------------------------------ */

class SceneSavedEvent : public Event
{
public:
	SceneSavedEvent(const std::filesystem::path& filepath)
		:m_Filepath(filepath)
	{}

	std::filesystem::path GetFilePath() const { return m_Filepath; }

	std::string to_string() const override
	{
		std::stringstream ss;
		ss << "Scene saved to: " << m_Filepath.string();
		return ss.str();
	}

	EVENT_CLASS_TYPE(SCENE_SAVED);
	EVENT_CLASS_CATEGORY(EventCategory::SCENE);
private:
	const std::filesystem::path m_Filepath;
};

/* ------------------------------------------------------------------------------------------------------------------ */

class SceneLoadedEvent : public Event
{
public:
	SceneLoadedEvent(const std::filesystem::path& filepath)
		:m_Filepath(filepath)
	{}

	std::filesystem::path GetFilePath() const { return m_Filepath; }

	std::string to_string() const override
	{
		std::stringstream ss;
		ss << "Scene loaded from : " << m_Filepath.string();
		return ss.str();
	}

	EVENT_CLASS_TYPE(SCENE_SAVED);
	EVENT_CLASS_CATEGORY(EventCategory::SCENE);
private:
	const std::filesystem::path m_Filepath;
};

/* ------------------------------------------------------------------------------------------------------------------ */

class SceneStateChangedEvent : public Event
{
public:
	SceneStateChangedEvent(SceneState state)
		:m_State(state)
	{
	}
	SceneState GetSceneState() const { return m_State; }
	std::string to_string() const override
	{
		std::stringstream ss;
		ss << "Scene state changed to: " << static_cast<int>(m_State);
		return ss.str();
	}
	EVENT_CLASS_TYPE(SCENE_STATE_CHANGED);
	EVENT_CLASS_CATEGORY(EventCategory::SCENE);
private:
	SceneState m_State;
};
