#pragma once
#include <string>

#include "cereal/cereal.hpp"

struct NameComponent
{
	std::string name;

	NameComponent() = default;
	NameComponent(const NameComponent& name) = default;
	NameComponent(const std::string& name)
		:name(name) {}

	operator std::string& () { return name; }
	operator const std::string& () const { return name; }
private:
	friend cereal::access;
	template<typename Archive>
	void serialize(Archive& archive)
	{
		archive(name);
	}
};