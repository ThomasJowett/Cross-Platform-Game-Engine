#pragma once

#include "Core/UUID.h"

#include "cereal/cereal.hpp"

struct IDComponent
{
	Uuid ID;

	IDComponent() = default;
	IDComponent(const IDComponent&)
		:ID() {} // Id components must be unique so cannot be copied
	IDComponent(Uuid id)
		:ID(id) {}
private:
	friend cereal::access;
	template<typename Archive>
	void serialize(Archive& archive)
	{
		archive(cereal::make_nvp("ID", (uint64_t)ID));
	}
};
