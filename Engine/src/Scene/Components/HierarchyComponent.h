#pragma once
#include "Scene/Entity.h"

struct HierarchyComponent
{
	entt::entity parent;
	entt::entity firstChild;
	entt::entity previousSibling;
	entt::entity nextSibling;
	bool isActive;

private:
	friend cereal::access;
	template<typename Archive>
	void serialize(Archive& archive)
	{
		archive(cereal::make_nvp("Parent", parent));
		archive(cereal::make_nvp("First Child", firstChild));
		archive(cereal::make_nvp("Previous Sibling", previousSibling));
		archive(cereal::make_nvp("Next Sibling", nextSibling));
	}
};
