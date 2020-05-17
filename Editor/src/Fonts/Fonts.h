#pragma once
struct ImFont;

class Fonts
{
public:
	static void LoadFonts();

	static ImFont* DroidSans;
	static ImFont* Consolas;
};