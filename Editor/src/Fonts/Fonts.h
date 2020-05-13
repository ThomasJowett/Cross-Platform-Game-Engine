#pragma once

#include "imgui/imgui.h"

#include "Fonts/DroidSans.cpp"
#include "Fonts/Consolas.cpp"

class Fonts
{
public:
	static void LoadFonts();

	static ImFont* DroidSans;
	static ImFont* Consolas;
};