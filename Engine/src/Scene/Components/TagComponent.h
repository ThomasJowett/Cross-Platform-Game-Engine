#pragma once
#include <string>

#include "cereal/cereal.hpp"

struct TagComponent
{
	std::string tag;

	TagComponent() = default;
	TagComponent(const TagComponent& tag) = default;
	TagComponent(const std::string& tag)
		:tag(tag) {}

	operator std::string& () { return tag; }
	operator const std::string& () const { return tag; }

	template<typename Archive>
	void serialize(Archive& archive)
	{
		archive(cereal::make_nvp("Tag", tag));
	}
};