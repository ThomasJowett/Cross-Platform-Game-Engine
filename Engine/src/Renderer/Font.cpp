#include "stdafx.h"
#include "Font.h"

Font::Font()
{
}

Font::Font(const std::filesystem::path& filepath)
{
	Load(filepath);
}

Ref<Font> Font::s_DefaultFont;

void Font::Init()
{
	s_DefaultFont = CreateRef<Font>(Application::GetWorkingDirectory() / "resources" / "Fonts" / "Roboto-Medium.tff");
}

void Font::Shutdown()
{
	s_DefaultFont.reset();
}

bool Font::Load(const std::filesystem::path& filepath)
{
	return true;
}