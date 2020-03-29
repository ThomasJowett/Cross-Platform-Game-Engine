#pragma once
#include "spdlog/sinks/base_sink.h"
#include "ImGuiConsole.h"
#include <mutex>

template<class Mutex>
class ImGuiConsoleSink : public spdlog::sinks::base_sink<std::mutex>
{
public:
	explicit ImGuiConsoleSink()
	{
	}

	ImGuiConsoleSink(const ImGuiConsoleSink&) = delete;
	ImGuiConsoleSink& operator=(const ImGuiConsoleSink&) = delete;
	virtual ~ImGuiConsoleSink() = default;
protected:
	void sink_it_(const spdlog::details::log_msg& msg) override
	{
		spdlog::memory_buf_t formatted;
		base_sink<Mutex>::formatter_->format(msg, formatted);

		ImGuiConsole::AddMessage(fmt::to_string(formatted), GetMessageLevel(msg.level));
	}

	void flush_() override
	{
	}

private:
	static ImGuiConsole::Message::Level GetMessageLevel(const spdlog::level::level_enum level)
	{
		switch (level)
		{
		case spdlog::level::level_enum::trace: return ImGuiConsole::Message::Level::Trace;
		case spdlog::level::level_enum::debug:return ImGuiConsole::Message::Level::Debug;
		case spdlog::level::level_enum::info:return ImGuiConsole::Message::Level::Info;
		case spdlog::level::level_enum::warn:return ImGuiConsole::Message::Level::Warn;
		case spdlog::level::level_enum::err:return ImGuiConsole::Message::Level::Error;
		case spdlog::level::level_enum::critical:return ImGuiConsole::Message::Level::Critical;
		case spdlog::level::level_enum::off:return ImGuiConsole::Message::Level::Off;
		default:
			break;
		}

		return ImGuiConsole::Message::Level::Invalid;
	}
};

using ImGuiConsoleSink_mt = ImGuiConsoleSink<std::mutex>;