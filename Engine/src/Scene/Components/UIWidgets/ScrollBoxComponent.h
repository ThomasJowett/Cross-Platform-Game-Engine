#pragma once

#include "cereal/cereal.hpp"

#include "math/Vector2f.h"

// Expects exactly one child as its scrollable content
// offsets that child, doesn't arrange multiple children itself.
struct ScrollBoxComponent
{
	ScrollBoxComponent() = default;
	ScrollBoxComponent(const ScrollBoxComponent&) = default;

	Vector2f scrollOffset = Vector2f(0.0f, 0.0f);
	bool horizontalScroll = false;
	bool verticalScroll = true;
	bool clipContent = true;

	// Derived each frame from the content child, used to clamp scrollOffset - not persisted.
	Vector2f contentSize = Vector2f(0.0f, 0.0f);

private:
	friend cereal::access;
	template<typename Archive>
	void serialize(Archive& archive)
	{
		archive(scrollOffset, horizontalScroll, verticalScroll, clipContent);
	}
};
