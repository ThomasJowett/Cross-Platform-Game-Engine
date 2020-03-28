#pragma once

#include <iostream>
#include <string>

#include "Core/core.h"


enum LoggingOutputs
{
	LO_NONE		= 0,
	LO_IDE		= BIT(0),
	LO_CONSOLE	= BIT(1),
	LO_IMGUI	= BIT(2),
	LO_FILE		= BIT(3)
};

enum class Level
{
	debug,
	error,
	warning,
	info
};

class Logger
{
public:
	static void Init();


};