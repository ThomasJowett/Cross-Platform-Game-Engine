#pragma once

#include "cereal/cereal.hpp"
#include "Renderer/Font.h"

struct TextComponent
{
	TextComponent() = default;
	TextComponent(const TextComponent&) = default;

	std::string text;
	Ref<Font> font = Font::GetDefaultFont();
	float maxWidth = 10.0f;
	Colour colour{ Colours::WHITE };

private:
	friend cereal::access;
	template<typename Archive>
	void save(Archive& archive) const
	{
		archive(text, maxWidth, colour);
		std::string relativePath;
		if(font != Font::GetDefaultFont())
			relativePath = FileUtils::RelativePath(font->GetFilepath(), Application::GetOpenDocumentDirectory()).string();
		archive(relativePath);
	}

	template<typename Archive>
	void load(Archive& archive)
	{
		archive(text, maxWidth, colour);
		std::string relativePath;
		archive(relativePath);
		if (!relativePath.empty())
		{
			font = AssetManager::GetAsset<Font>(std::filesystem::absolute(Application::GetOpenDocumentDirectory() / relativePath));
		}
		else
		{
			font = Font::GetDefaultFont();
		}
	}
};
