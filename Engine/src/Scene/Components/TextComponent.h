#pragma once

#include "cereal/cereal.hpp"

struct TextComponent
{
	TextComponent() = default;
	TextComponent(const TextComponent&) = default;

	std::string text;
	uint32_t fontSize;

private:
	friend cereal::access;
	template<typename Archive>
	void serialize(Archive& archive)
	{
		archive(text, fontSize);
	}
};
