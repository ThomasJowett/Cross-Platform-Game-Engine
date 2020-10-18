#pragma once

#include <string>

#include "cereal/cereal.hpp"

struct ProjectData
{
	std::string DefaultScene;

	std::string Description;

	template<typename Archive>
	void serialize(Archive& archive)
	{
		archive(cereal::make_nvp("Default Scene", DefaultScene));
		archive(cereal::make_nvp("Description", Description));
	}
};