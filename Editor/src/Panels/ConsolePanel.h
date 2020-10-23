#pragma once
#include "Core/core.h"
#include "Core/Colour.h"

#include "Core/Layer.h"

#include "Logging/InternalConsoleSink.h"

struct ImGuiTextFilter;

class ConsolePanel
	:public Layer
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

public:
	static void Clear();
	static void SetDarkTheme(bool darktheme);

	ConsolePanel(bool* show);
	~ConsolePanel() = default;
	void OnImGuiRender() override;
private:
	void ImGuiRenderHeader();
	void ImGuiRenderSettings();
	void ImGuiRenderMessages();
	void RenderMessage(const InternalConsoleSink_mt::Message& message);

	Level GetMessageLevel(spdlog::level::level_enum level);

	static Level GetLowerLevel(Level level);
	static Level GetHigherLevel(Level level);
	static const char* GetLevelName(Level level);
	static Colour GetRenderColour(Level level);

private:
	bool* m_Show;

	float m_DisplayScale;
	bool m_AllowScrollingToBottom;
	uint16_t m_LastBufferSize;

	Level m_LevelFilter;
	ImGuiTextFilter* m_TextFilter;

	static bool s_DarkTheme;
	static std::vector<Level> s_Levels;
};
