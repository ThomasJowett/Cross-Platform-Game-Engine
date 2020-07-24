#pragma once

#include <iostream>
#include <string>

#include "Core/core.h"

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

#include "Debug.h"


enum LoggingOutputs
{
	LO_NONE		= 0,
	LO_IDE		= BIT(0),
	LO_CONSOLE	= BIT(1),
	LO_IMGUI	= BIT(2),
	LO_FILE		= BIT(3)
};

class Logger
{
public:
	static void Init();

	inline static Ref<spdlog::logger>& GetEngineLogger() { return s_EngineLogger; }
	inline static Ref<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

private:
	static Ref<spdlog::logger> s_EngineLogger;
	static Ref<spdlog::logger> s_ClientLogger;
};

// Fatal error only to be called when the application is about to crash
#define ENGINE_CRITICAL(...){	::Logger::GetEngineLogger()->critical(__VA_ARGS__); DBG_OUTPUT("\n");}

// Serious issue and a failure of something important
#define ENGINE_ERROR(...){		::Logger::GetEngineLogger()->error(__VA_ARGS__); DBG_OUTPUT("\n");}

// Indicates you may have a problem or unusual situation
#define ENGINE_WARN(...){		::Logger::GetEngineLogger()->warn(__VA_ARGS__); DBG_OUTPUT("\n");}

// Normal application behaviour
#define ENGINE_INFO(...)		::Logger::GetEngineLogger()->info(__VA_ARGS__)

#ifdef DEBUG
// Diagnostic information to help the understand the flow of the engine
#define ENGINE_DEBUG(...){		::Logger::GetEngineLogger()->debug(__VA_ARGS__); DBG_OUTPUT("\n");}

// Very fine detailed Diagnostic information
#define ENGINE_TRACE(...)		::Logger::GetEngineLogger()->trace(__VA_ARGS__)

#else
// Nothing logged unless in debug mode
#define ENGINE_DEBUG(...)
// Nothing logged unless in debug mode
#define ENGINE_TRACE(...)
#endif // DEBUG

// Fatal error only to be called when the application is about to crash
#define CLIENT_CRITICAL(...){	::Logger::GetClientLogger()->critical(__VA_ARGS__); DBG_OUTPUT("\n");}

// Serious issue and a failure of something important
#define CLIENT_ERROR(...){		::Logger::GetClientLogger()->error(__VA_ARGS__); DBG_OUTPUT("\n");}

// Indicates you may have a problem or unusual situation
#define CLIENT_WARN(...)		::Logger::GetClientLogger()->warn(__VA_ARGS__)

// Normal application behaviour
#define CLIENT_INFO(...)		::Logger::GetClientLogger()->info(__VA_ARGS__)

#ifdef DEBUG

// Diagnostic information to help the understand the flow of the engine
#define CLIENT_DEBUG(...){		::Logger::GetClientLogger()->debug(__VA_ARGS__); DBG_OUTPUT("\n");}

// Very fine detailed Diagnostic information
#define CLIENT_TRACE(...)		::Logger::GetClientLogger()->trace(__VA_ARGS__)

#else
// Nothing logged unless in debug mode
#define CLIENT_DEBUG(...)
// Nothin logged unless in debug mode
#define CLIENT_TRACE(...)
#endif // DEBUG