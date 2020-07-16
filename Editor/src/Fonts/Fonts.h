#pragma once
struct ImFont;

class Fonts
{
public:
	static void LoadFonts(float fontSize = 16.0f);

	static ImFont* DroidSans;
	static ImFont* Consolas;
};