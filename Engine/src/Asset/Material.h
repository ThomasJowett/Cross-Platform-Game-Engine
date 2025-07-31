#pragma once

#include "Asset/Shader.h"
#include "Texture.h"
#include "Core/Asset.h"

#include "cereal/cereal.hpp"
#include "cereal/access.hpp"
#include "Core/Application.h"

#include "Utilities/FileUtils.h"

namespace tinyxml2
{
class XMLDocument;
}

class Material : public Asset
{
public:
	Material();
	Material(const std::filesystem::path& filepath);
	Material(const std::filesystem::path& filepath, const std::vector<uint8_t>& data);
	Material(const std::string& shader, Colour tint = Colours::WHITE);

	const std::string& GetShader() const { return m_Shader; }
	void SetShader(const std::string& shader) { m_Shader = shader; }
	void BindTextures() const;
	Ref<Texture2D> GetTexture(uint32_t slot);

	void AddTexture(Ref<Texture2D>, uint32_t slot);

	void SetTint(const Colour& tint) { m_Tint = tint; }
	Colour GetTint() const { return m_Tint; }

	virtual bool Load(const std::filesystem::path& filepath) override;
	virtual bool Load(const std::filesystem::path& filepath, const std::vector<uint8_t>& data) override;

	bool SaveMaterial(const std::filesystem::path& filepath) const;
	bool SaveMaterial() const;

	bool IsTwoSided() const { return m_TwoSided; }
	void SetTwoSided(bool twoSided) { m_TwoSided = twoSided; }

	bool IsTransparent() const { return m_Transparent; }
	void SetTransparency(bool transparent) { m_Transparent = transparent; }

	bool CastsShadows() const { return m_CastShadows; }
	void SetCastShadows(bool castShadows) { m_CastShadows = castShadows; }

	float GetTilingFactor() const { return m_TilingFactor; }
	void SetTilingFactor(float tilingFactor) { m_TilingFactor = tilingFactor; }

	Vector2f GetTextureOffset() { return m_TextureOffset; }
	void SetTextureOffset(Vector2f textureOffset) { m_TextureOffset = textureOffset; }

	static Ref<Material> GetDefaultMaterial();

private:

	std::string m_Shader = "Standard";

	std::unordered_map<uint32_t, Ref<Texture2D>> m_Textures;

	Colour m_Tint{ Colours::WHITE };

	bool m_TwoSided = false;
	bool m_Transparent = false;
	bool m_CastShadows = true;

	float m_TilingFactor = 1.0f;
	Vector2f m_TextureOffset;

	static Ref<Material> s_DefaultMaterial;

	bool LoadXML(tinyxml2::XMLDocument* doc);
};