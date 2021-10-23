#pragma once

#include "Core/UUID.h"

struct IDComponent
{
	Uuid ID;

	IDComponent() = default;
	IDComponent(const IDComponent&) = default;
	IDComponent(Uuid id)
		:ID(id) {}

	template<typename Archive>
	void serialize(Archive& archive)
	{
		archive(cereal::make_nvp("ID", (uint64_t)ID));
	}
};
