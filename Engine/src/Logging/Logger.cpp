#include "stdafx.h"
#include "Logger.h"
#include "Core/Version.h"

#include <spdlog/sinks/stdout_color_sinks.h>

#include <spdlog/sinks/basic_file_sink.h>
#include <ImGui/ImGuiConsoleSink.h>
#include <spdlog/sinks/msvc_sink.h>

Ref<spdlog::logger> Logger::s_EngineLogger;
Ref<spdlog::logger> Logger::s_ClientLogger;

void Logger::Init()
{
	PROFILE_FUNCTION();

	std::vector<spdlog::sink_ptr>logSinks;
	logSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());					// std::cout
	logSinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("Log.txt", true));	// file
	logSinks.emplace_back(std::make_shared<ImGuiConsoleSink_mt>());									// ImGuiConsole
#ifdef _MSC_VER
	logSinks.emplace_back(std::make_shared<spdlog::sinks::msvc_sink_mt>());							// msvc output
	logSinks[3]->set_pattern("%n: %v");
#endif

	logSinks[0]->set_pattern("%^[%T] %n: %v%$");
	logSinks[1]->set_pattern("[%d/%m/%Y] [%T] [%l] %n: %v");
	logSinks[2]->set_pattern("%^[%T] [%l] %n: %v%$");

	s_EngineLogger = std::make_shared<spdlog::logger>("ENGINE", begin(logSinks), end(logSinks));
	spdlog::register_logger(s_EngineLogger);
	s_EngineLogger->set_level(spdlog::level::trace);
	s_EngineLogger->flush_on(spdlog::level::trace);

	s_ClientLogger = std::make_shared<spdlog::logger>("CLIENT", begin(logSinks), end(logSinks));
	spdlog::register_logger(s_ClientLogger);
	s_ClientLogger->set_level(spdlog::level::trace);
	s_ClientLogger->flush_on(spdlog::level::trace);

	ENGINE_INFO("Version: {0}", VERSION);
}
