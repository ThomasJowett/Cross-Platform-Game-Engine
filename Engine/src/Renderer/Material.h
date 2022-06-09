#pragma once

#include "Shader.h"
#include "Texture.h"
#include "Asset.h"

#include "cereal/cereal.hpp"
#include "cereal/access.hpp"
#include "Core/Application.h"

#include "Utilities/FileUtils.h"

class Material : public Asset
{
public:
	Material() = default;
	Material(const std::filesystem::path& filepath);
	Material(const std::string& shader, Colour tint = Colours::WHITE);

	const std::string& GetShader() const { return m_Shader; }
	void SetShader(const std::string& shader) { m_Shader = shader; }
	void BindTextures() const;
	Ref<Texture2D> GetTexture(uint32_t slot);

	void AddTexture(Ref<Texture2D>, uint32_t slot);

	void SetTint(const Colour& tint) { m_Tint = tint; }
	Colour GetTint() const { return m_Tint; }

	virtual bool Load(const std::filesystem::path& filepath) override;

	bool SaveMaterial(const std::filesystem::path& filepath) const;
	bool SaveMaterial() const;

	const std::filesystem::path& GetFilepath() const { return m_Filepath; }
private:
	std::string m_Shader;

	std::unordered_map<uint32_t, Ref<Texture2D>> m_Textures;

	Colour m_Tint;

	std::filesystem::path m_Filepath;
};