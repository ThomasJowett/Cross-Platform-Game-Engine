#pragma once

#include "Shader.h"
#include "Texture.h"

class Material
{
public:
	Material(Ref<Shader> shader);

	const Ref<Shader> GetShader() const { return m_Shader; };
	void BindTextures() const;

	void AddTexture(Ref<Texture>, uint32_t slot);
private:
	Ref<Shader> m_Shader;

	std::unordered_map<Ref<Texture>, uint32_t> m_Textures;
};