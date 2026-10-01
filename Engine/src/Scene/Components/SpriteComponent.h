#pragma once

#include "Core/Colour.h"

#include "cereal/cereal.hpp"
#include "cereal/access.hpp"

#include "Asset/Texture.h"
#include "Asset/Material.h"
#include "Scene/AssetManager.h"
#include "Utilities/SerializationUtils.h"
#include "Scripting/Lua/LuaBindings.h"

struct SpriteComponent
{
	Colour tint{ 1.0f, 1.0f,1.0f,1.0f };
	Ref<Texture2D> texture;
	float tilingFactor = 1.0f;

	std::filesystem::path texturePath;

	SpriteComponent() = default;
	SpriteComponent(const SpriteComponent&) = default;

	REFLECT_LUA_BEGIN(SpriteComponent)
		REFLECT_LUA_PROPERTY_CUSTOM("Tint", "Colour multiplied over the texture", "Colour",
			([](Self& c) -> Colour& { return c.tint; }),
			([](Self& c, const Colour& v) { c.tint = v; }))
		REFLECT_LUA_PROPERTY_CUSTOM("Texture", "The texture drawn on the sprite, or nil for a plain coloured quad", "Texture2D",
			([](Self& c) { return c.texture; }),
			([](Self& c, const Ref<Texture2D>& v) { c.texture = v; c.texturePath = v ? v->GetFilepath() : std::filesystem::path(); }))
		REFLECT_LUA_PROPERTY_CUSTOM("TilingFactor", "How many times the texture repeats across the sprite", "number",
			([](Self& c) { return c.tilingFactor; }),
			([](Self& c, float v) { c.tilingFactor = v; }))
	REFLECT_LUA_END()

private:
	friend cereal::access;
	template<typename Archive>
	void save(Archive& archive) const
	{
		SerializationUtils::SaveTextureToArchive(archive, texture);
		archive(tint);
		archive(tilingFactor);
		archive(texturePath.string());
	}

	template<typename Archive>
	void load(Archive& archive)
	{
		SerializationUtils::LoadTextureFromArchive(archive, texture);
		archive(tint);
		archive(tilingFactor);
		std::string path;
		archive(path);
		texturePath = path;
	}
};
