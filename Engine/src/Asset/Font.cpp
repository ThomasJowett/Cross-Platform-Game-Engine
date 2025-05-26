#include "stdafx.h"
#include "Font.h"

#include "Renderer/UI/MSDFData.h"
#include "Logging/Instrumentor.h"
#include "Scene/AssetManager.h"

Font::Font()
{
}

Font::Font(const std::filesystem::path& filepath)
{
	Load(filepath);
}

Font::Font(const std::filesystem::path& filepath, const std::vector<uint8_t>& data)
{
	Load(filepath, data);
}

Font::~Font()
{
	delete m_MSDFData;
}

Ref<Font> Font::s_DefaultFont;

void Font::Init()
{
	PROFILE_FUNCTION();
	if (AssetManager::HasBundle())
	{
		std::vector<uint8_t> data;
		AssetManager::GetFileData("data/Fonts/Manrope-Medium.ttf", data);
		s_DefaultFont = CreateRef<Font>("data/Fonts/Manrope-Medium.ttf", data);
	}
	else
		s_DefaultFont = CreateRef<Font>("data/Fonts/Manrope-Medium.ttf");
}

void Font::Shutdown()
{
	s_DefaultFont.reset();
}

bool Font::Load(const std::filesystem::path& filepath)
{
	PROFILE_FUNCTION();
	std::filesystem::path absolutePath = std::filesystem::absolute(Application::GetOpenDocumentDirectory() / filepath);
	if (!std::filesystem::exists(absolutePath))
	{
		ENGINE_ERROR("Font does not exist: {0}", absolutePath);
		return false;
	}

	m_TextureAtlas.reset();

	// Try Loading the atlas cache
	std::filesystem::path cachePath = filepath;
	cachePath.replace_extension(".png");
	std::filesystem::path absoluteCachePath = std::filesystem::absolute(Application::GetOpenDocumentDirectory() / cachePath);
	if (std::filesystem::exists(absoluteCachePath))
	{
		m_TextureAtlas = AssetManager::GetTexture(cachePath);
		m_TextureAtlas->SetFilterMethod(Texture::FilterMethod::Linear);
	}

	msdfgen::FreetypeHandle* ftHandle = msdfgen::initializeFreetype();
	msdfgen::FontHandle* fontHandle = msdfgen::loadFont(ftHandle, absolutePath.string().c_str());

	if (!ftHandle || !fontHandle)
	{
		ENGINE_ERROR("Could not load font: {0}", filepath);
		msdfgen::destroyFont(fontHandle);
		msdfgen::deinitializeFreetype(ftHandle);
		return false;
	}	

	m_Filepath = filepath;
	m_Filepath.make_preferred();

	if (m_MSDFData)
		delete m_MSDFData;
	m_MSDFData = new MSDFData();
	m_MSDFData->fontGeometry = msdf_atlas::FontGeometry(&m_MSDFData->glyphs);

	int glyphsLoaded = -1;

	glyphsLoaded = m_MSDFData->fontGeometry.loadCharset(fontHandle, 1.0f, msdf_atlas::Charset::ASCII);

	ASSERT(glyphsLoaded >= 0, "Could not load any glyphs from the font");

	m_MSDFData->fontGeometry.setName(m_Filepath.string().c_str());

	for (msdf_atlas::GlyphGeometry& glyph : m_MSDFData->glyphs)
	{
		glyph.edgeColoring(msdfgen::edgeColoringInkTrap, 3.0, 0);
	}

	msdf_atlas::TightAtlasPacker atlasPacker;

	atlasPacker.setDimensionsConstraint(msdf_atlas::TightAtlasPacker::DimensionsConstraint::MULTIPLE_OF_FOUR_SQUARE);
	atlasPacker.setPadding(0);
	atlasPacker.setScale(40);
	atlasPacker.setPixelRange(2.0);
	atlasPacker.setMiterLimit(1.0);
	if (int remaining = atlasPacker.pack(m_MSDFData->glyphs.data(), (int)m_MSDFData->glyphs.size()))
	{
		ASSERT(remaining >= 0, "Remaining cannot be negative");
		ENGINE_ERROR("Could not fit {0} out of {1} glyphs into the atlas.", remaining, (int)m_MSDFData->glyphs.size());
	}

	if (m_TextureAtlas)
		return true;

	int width = 0, height = 0;
	atlasPacker.getDimensions(width, height);
	ASSERT(width > 0 && height > 0, "Area of font atlas cannot be zero");
	ENGINE_TRACE("Generated font atlas with dimensions: {0} x {1}", width, height);

	const int bytes = 4;

	msdf_atlas::ImmediateAtlasGenerator<float, bytes, msdf_atlas::mtsdfGenerator, msdf_atlas::BitmapAtlasStorage<float, bytes>> generator(width, height);

	msdf_atlas::GeneratorAttributes generatorAttributes;
	generatorAttributes.config.overlapSupport = true;
	generatorAttributes.scanlinePass = true;
	generator.setAttributes(generatorAttributes);
	generator.setThreadCount(8);
	generator.generate(m_MSDFData->glyphs.data(), (int)m_MSDFData->glyphs.size());

	msdfgen::BitmapConstRef<float, bytes> bitmap = (msdfgen::BitmapConstRef<float, bytes>)generator.atlasStorage();

	msdfgen::destroyFont(fontHandle);
	msdfgen::deinitializeFreetype(ftHandle);

	// Cache the texture to png file
	msdfgen::savePng(bitmap, absoluteCachePath.string().c_str());

	m_TextureAtlas = AssetManager::GetTexture(cachePath);

	m_TextureAtlas->SetFilterMethod(Texture::FilterMethod::Linear);
	return true;
}

bool Font::Load(const std::filesystem::path& filepath, const std::vector<uint8_t>& data)
{
	PROFILE_FUNCTION();

	msdfgen::FreetypeHandle* ftHandle = msdfgen::initializeFreetype();
	msdfgen::FontHandle* fontHandle = msdfgen::loadFontData(ftHandle, data.data(), (int)data.size());

	if (!ftHandle || !fontHandle)
	{
		ENGINE_ERROR("Could not load font: {0}", filepath);
		msdfgen::destroyFont(fontHandle);
		msdfgen::deinitializeFreetype(ftHandle);
		return false;
	}

	m_Filepath = filepath;
	m_Filepath.make_preferred();
	m_TextureAtlas.reset();

	// Load PNG atlas from memory
	std::filesystem::path cachePath = filepath;
	cachePath.replace_extension(".png");

	std::vector<uint8_t> pngData;
	AssetManager::GetFileData(cachePath, pngData);
	if (pngData.empty())
	{
		ENGINE_ERROR("Could not load font atlas from memory: {0}", cachePath.string());
		msdfgen::destroyFont(fontHandle);
		msdfgen::deinitializeFreetype(ftHandle);
		return false;
	}

	m_TextureAtlas = AssetManager::GetTexture(cachePath);
	m_TextureAtlas->SetFilterMethod(Texture::FilterMethod::Linear);

	if (m_MSDFData)
		delete m_MSDFData;
	m_MSDFData = new MSDFData();
	m_MSDFData->fontGeometry = msdf_atlas::FontGeometry(&m_MSDFData->glyphs);

	int glyphsLoaded = m_MSDFData->fontGeometry.loadCharset(fontHandle, 1.0f, msdf_atlas::Charset::ASCII);
	ASSERT(glyphsLoaded >= 0, "Could not load any glyphs from the font");

	m_MSDFData->fontGeometry.setName(m_Filepath.string().c_str());

	for (msdf_atlas::GlyphGeometry& glyph : m_MSDFData->glyphs)
	{
		glyph.edgeColoring(msdfgen::edgeColoringInkTrap, 3.0, 0);
	}

	msdf_atlas::TightAtlasPacker atlasPacker;

	atlasPacker.setDimensionsConstraint(msdf_atlas::TightAtlasPacker::DimensionsConstraint::MULTIPLE_OF_FOUR_SQUARE);
	atlasPacker.setPadding(0);
	atlasPacker.setScale(40);
	atlasPacker.setPixelRange(2.0);
	atlasPacker.setMiterLimit(1.0);
	if (int remaining = atlasPacker.pack(m_MSDFData->glyphs.data(), (int)m_MSDFData->glyphs.size()))
	{
		ASSERT(remaining >= 0, "Remaining cannot be negative");
		ENGINE_ERROR("Could not fit {0} out of {1} glyphs into the atlas.", remaining, (int)m_MSDFData->glyphs.size());
	}

	msdfgen::destroyFont(fontHandle);
	msdfgen::deinitializeFreetype(ftHandle);

	return true;
}
