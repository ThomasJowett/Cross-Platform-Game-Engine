#include "stdafx.h"
#include "Logger.h"

/*

Logger* Logger::s_Instance = nullptr;

Logger::Logger()
{
	CORE_ASSERT(!s_Instance, "Logger already exists cannot create multiple loggers");
	s_Instance = this;

	consoleStream = &std::cout;
}

Logger& Logger::GetInstance()
{
	if (!s_Instance)
		return *new Logger();
	return *s_Instance;
}
*/