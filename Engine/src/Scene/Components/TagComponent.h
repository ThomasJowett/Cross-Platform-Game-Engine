#pragma once
#include <string>

struct TagComponent
{
	std::string Tag;

	TagComponent() = default;
	TagComponent(const TagComponent& tag) = default;
	TagComponent(const std::string& tag)
		:Tag(tag) {}

	operator std::string& () { return Tag; }
	operator const std::string& () const { return Tag; }
};