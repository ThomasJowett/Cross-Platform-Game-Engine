#pragma once
#include "Scene/Entity.h"

struct HierarchyComponent
{
	entt::entity parent = entt::null;
	entt::entity firstChild = entt::null;
	entt::entity previousSibling = entt::null;
	entt::entity nextSibling = entt::null;
	bool isActive = true;

private:
	friend cereal::access;
	template<typename Archive>
	void serialize(Archive& archive)
	{
		archive(parent, firstChild, previousSibling, nextSibling);
	}
};
