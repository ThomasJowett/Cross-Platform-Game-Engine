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
	Texture::FilterMethod filterMethod = Texture::FilterMethod::Nearest;
	Texture::WrapMethod wrapMethod = Texture::WrapMethod::Repeat;

	SpriteComponent() = default;
	SpriteComponent(const SpriteComponent&) = default;

	// Loads the texture from texturePath on first use; the scene loader leaves it unloaded when the sprite atlas covers it
	const Ref<Texture2D>& GetTexture()
	{
		if (!texture && !texturePath.empty())
			texture = AssetManager::GetTexture(texturePath);
		return texture;
	}

	REFLECT_LUA_BEGIN(SpriteComponent)
		Lua::SetEnum(state, "FilterMethod", "Texture filtering, for SpriteComponent.FilterMethod", {
			{ "Linear", (int)Texture::FilterMethod::Linear },
			{ "Nearest", (int)Texture::FilterMethod::Nearest }
		});
		Lua::SetEnum(state, "WrapMethod", "Texture wrapping outside 0-1, for SpriteComponent.WrapMethod", {
			{ "Clamp", (int)Texture::WrapMethod::Clamp },
			{ "Mirror", (int)Texture::WrapMethod::Mirror },
			{ "Repeat", (int)Texture::WrapMethod::Repeat }
		});
		REFLECT_LUA_PROPERTY_CUSTOM("Tint", "Colour multiplied over the texture", "Colour",
			([](Self& c) -> Colour& { return c.tint; }),
			([](Self& c, const Colour& v) { c.tint = v; }))
		REFLECT_LUA_PROPERTY_CUSTOM("Texture", "The texture drawn on the sprite, or nil for a plain coloured quad", "Texture2D",
			([](Self& c) { return c.GetTexture(); }),
			([](Self& c, const Ref<Texture2D>& v) { c.texture = v; c.texturePath = v ? v->GetFilepath() : std::filesystem::path(); }))
		REFLECT_LUA_PROPERTY_CUSTOM("FilterMethod", "FilterMethod.Linear or FilterMethod.Nearest", "FilterMethod",
			([](Self& c) { return c.filterMethod; }),
			([](Self& c, Texture::FilterMethod v) { c.filterMethod = v; }))
		REFLECT_LUA_PROPERTY_CUSTOM("WrapMethod", "WrapMethod.Clamp, WrapMethod.Mirror or WrapMethod.Repeat", "WrapMethod",
			([](Self& c) { return c.wrapMethod; }),
			([](Self& c, Texture::WrapMethod v) { c.wrapMethod = v; }))
		REFLECT_LUA_PROPERTY_CUSTOM("TilingFactor", "How many times the texture repeats across the sprite", "number",
			([](Self& c) { return c.tilingFactor; }),
			([](Self& c, float v) { c.tilingFactor = v; if (v != 1.0f) c.GetTexture(); }))
	REFLECT_LUA_END()

private:
	friend cereal::access;
	template<typename Archive>
	void save(Archive& archive) const
	{
		archive(texture != nullptr);
		archive(tint);
		archive(tilingFactor);
		archive(texturePath.string());
		archive((int)filterMethod);
		archive((int)wrapMethod);
	}

	template<typename Archive>
	void load(Archive& archive)
	{
		bool hasTexture;
		archive(hasTexture);
		archive(tint);
		archive(tilingFactor);
		std::string path;
		archive(path);
		texturePath = path;
		int filter, wrap;
		archive(filter);
		archive(wrap);
		filterMethod = (Texture::FilterMethod)filter;
		wrapMethod = (Texture::WrapMethod)wrap;
		texture = hasTexture && !texturePath.empty() ? AssetManager::GetTexture(texturePath) : nullptr;
	}
};
