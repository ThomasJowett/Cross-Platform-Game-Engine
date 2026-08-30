#pragma once

#include <string>

#include "cereal/cereal.hpp"

struct ProjectData
{
	std::string defaultScene;

	std::string description;

	uint32_t spriteAtlasPageSize = 2048;

	template<typename Archive>
	void serialize(Archive& archive)
	{
		archive(cereal::make_nvp("Default Scene", defaultScene));
		archive(cereal::make_nvp("Description", description));

		// try catch is needed for backwards compatibility
		// TODO: convert project files to xml like the rest of the game files
		try
		{
			archive(cereal::make_nvp("Sprite Atlas Page Size", spriteAtlasPageSize));
		}
		catch (const cereal::Exception&)
		{
			spriteAtlasPageSize = 2048;
		}
	}
};