#pragma once

#include "Shader.h"
#include "Texture.h"

#include "cereal/cereal.hpp"
#include "cereal/access.hpp"
#include "Core/Application.h"

#include "Utilities/FileUtils.h"

class Material
{
public:
	Material() = default;
	Material(Ref<Shader> shader, Colour tint = Colours::WHITE);

	const Ref<Shader> GetShader() const { return m_Shader; };
	void BindTextures() const;

	void AddTexture(Ref<Texture>, uint32_t slot);

	void SetTint(const Colour& tint) { m_Tint = tint; }
	Colour GetTint() const { return m_Tint; }

	std::filesystem::path& GetFilepath() { return m_Filepath; }

	void LoadMaterial(const std::filesystem::path& filepath);
	void LoadMaterial();

	bool SaveMaterial(const std::filesystem::path& filepath) const;
	bool SaveMaterial() const;
private:
	Ref<Shader> m_Shader;

	std::unordered_map<Ref<Texture>, uint32_t> m_Textures;

	Colour m_Tint;

	std::filesystem::path m_Filepath;

	friend cereal::access;
	template<typename Archive>
	void save(Archive& archive) const
	{
		std::string relativePath = FileUtils::relativePath(m_Filepath, Application::GetOpenDocumentDirectory());
		archive(cereal::make_nvp("Filepath", relativePath));
		SaveMaterial();
	}

	template<typename Archive>
	void load(Archive& archive)
	{
		std::string relativePath;
		archive(cereal::make_nvp("Filepath", relativePath));

		m_Filepath = std::filesystem::absolute(Application::GetOpenDocumentDirectory() / relativePath);
		LoadMaterial();
	}
};