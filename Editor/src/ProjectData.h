#pragma once

#include <string>

#include "cereal/cereal.hpp"

struct ProjectData
{
	std::string defaultScene;

	std::string description;

	template<typename Archive>
	void serialize(Archive& archive)
	{
		archive(cereal::make_nvp("Default Scene", defaultScene));
		archive(cereal::make_nvp("Description", description));
	}
};