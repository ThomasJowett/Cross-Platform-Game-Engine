#pragma once

#include "cereal/access.hpp"

struct WidgetComponent
{
	WidgetComponent() = default;
	WidgetComponent(const WidgetComponent& other) = default;

	Vector2f size;


	//bool OnPressed();
	//bool OnReleased();
	//bool OnHovered();
	//bool OnUnHovered();

private:
	friend cereal::access;
	template<typename Archive>
	void serialize(Archive& archive)
	{
		archive(size);
	}
};
