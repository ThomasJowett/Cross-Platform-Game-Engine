#pragma once

#include "cereal/cereal.hpp"
#include "Renderer/Font.h"

struct TextComponent
{
	TextComponent() = default;
	TextComponent(const TextComponent&) = default;

	std::string text;
	Ref<Font> font;

private:
	friend cereal::access;
	template<typename Archive>
	void save(Archive& archive) const
	{
		archive(text);
		std::string relativePath;
		relativePath = FileUtils::RelativePath(font->GetFilepath(), Application::GetOpenDocumentDirectory()).string();
		archive(relativePath);
	}

	template<typename Archive>
	void load(Archive& archive)
	{
		archive(text);
		std::string relativePath;
		archive(relativePath);
		if (!relativePath.empty())
		{
			font = AssetManager::GetAsset<Font>(std::filesystem::absolute(Application::GetOpenDocumentDirectory() / relativePath));
		}
		else
		{
			font.reset();
		}
	}
};
