#pragma once

#include "Core/UUID.h"

#include "cereal/cereal.hpp"

struct IDComponent
{
	Uuid ID;

	IDComponent() = default;
	IDComponent(const IDComponent& other)
		:ID(other.ID) {}
	IDComponent(Uuid id)
		:ID(id) {}
private:
	friend cereal::access;
    template <typename Archive>
    void save(Archive& archive) const
    {
        uint64_t uuid = (uint64_t)ID;
        archive(uuid);
    }

    template <typename Archive>
    void load(Archive& archive)
    {
        uint64_t uuid;
        archive(uuid);

        ID = Uuid(uuid);
    }
};
