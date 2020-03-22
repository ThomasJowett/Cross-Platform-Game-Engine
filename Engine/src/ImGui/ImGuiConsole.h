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
			Debug = 1,
			Warn = 3,
			Error = 4,
			Critical = 5,
			Off = 6, // Display nothing
		};

		Message(const std::string message = "", Level level = Level::Invalid);

		void OnImGuiRender();

		static Level GetLowerLevel(Level level);
		static Level GetHigherLevel(Level level);
		static const char* GetLevelName(Level level);

	private:
		static Colour GetRenderColour(Level level);
	public:
		const Level m_Level;
		static std::vector<Level> s_Levels;
	};

public:
	~ImGuiConsole() = default;
	static void AddMessage(Ref<Message> message);
	static void Clear();
	static void OnImGuiRender(bool* show);
protected:
	ImGuiConsole() = default;
private:
	struct ImGuiRendering
	{
		static void ImGuiRenderHeader();
		static void ImGuiRenderSettings();
		static void ImGuiRenderMessages();
	};

private:
	static float s_DisplayScale;
	static uint16_t s_MessageBufferCapacity;
	static uint16_t s_MessageBufferSize;
	static uint16_t s_MessageBufferBegin;
	static Message::Level s_MessageBufferRenderFilter;
	static std::vector<Ref<Message>> s_MessageBuffer;
	static bool s_AllowScrollingToBottom;
	static bool s_RequestScrollToBottom;
};
