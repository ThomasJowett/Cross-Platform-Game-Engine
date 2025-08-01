#pragma once

#include "Core/Asset.h"
#include "cereal/access.hpp"
#include "Texture.h"

struct MSDFData;

class Font : public Asset
{
public:
	Font();
	Font(const std::filesystem::path& filepath);
	Font(const std::filesystem::path& filepath, const std::vector<uint8_t>& data);
	~Font();

	// Inherited via Asset
	virtual bool Load(const std::filesystem::path& filepath) override;
	virtual bool Load(const std::filesystem::path& filepath, const std::vector<uint8_t>& data) override;

	Ref<Texture2D> GetFontAtlas() const { return m_TextureAtlas; }
	const MSDFData* GetMSDFData() const { return m_MSDFData; }

	static void Init();
	static void Shutdown();

	static Ref<Font> GetDefaultFont() { return s_DefaultFont; }
private:
	Ref<Texture2D> m_TextureAtlas;
	MSDFData* m_MSDFData = nullptr;

	static Ref<Font> s_DefaultFont;
};