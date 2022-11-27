#include "stdafx.h"
#include "Font.h"

Font::Font()
{
}

Font::Font(const std::filesystem::path& filepath)
{
	Load(filepath);
}

bool Font::Load(const std::filesystem::path& filepath)
{
	return true;
}