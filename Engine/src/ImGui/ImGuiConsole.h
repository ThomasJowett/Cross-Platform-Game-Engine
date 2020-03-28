#pragma once
#include "Core/core.h"
#include "Core/Colour.h"
class ImGuiConsole
{
public:
	class Message
	{
	public:

		enum class Level
		{
			Invalid = -1,
			Trace = 0,
			Info = 1,
			Debug = 2,
			Warn = 3,
			Error = 4,
			Critical = 5,
			Off = 6, // Display nothing
		};

		Message(const std::string& message = "", Level level = Level::Invalid);

		void OnImGuiRender(Level filter);

		static Level GetLowerLevel(Level level);
		static Level GetHigherLevel(Level level);
		static const char* GetLevelName(Level level);

	private:
		static Colour GetRenderColour(Level level);
	public:
		std::string m_Message;
		const Level m_Level;
		static std::vector<Level> s_Levels;
	};

public:
	static void AddMessage(const std::string& message, Message::Level level);
	static void Clear();

	~ImGuiConsole() = default;
	void OnImGuiRender(bool* show);
	ImGuiConsole();
protected:
private:
	void ImGuiRenderHeader();
	void ImGuiRenderSettings();
	void ImGuiRenderMessages();

private:
	float m_DisplayScale;
	Message::Level m_MessageBufferRenderFilter;
	bool m_AllowScrollingToBottom;
	bool m_RequestScrollToBottom;

	static uint16_t s_MessageBufferCapacity;
	static uint16_t s_MessageBufferSize;
	static uint16_t s_MessageBufferBegin;
	static std::vector<Ref<Message>> s_MessageBuffer;
};
