#include "stdafx.h"
#include "Font.h"

#include "UI/MSDFData.h"

Font::Font()
{
}

Font::Font(const std::filesystem::path& filepath)
{
	Load(filepath);
}

Font::~Font()
{
	delete m_MSDFData;
}

Ref<Font> Font::s_DefaultFont;

void Font::Init()
{
	PROFILE_FUNCTION();
	s_DefaultFont = CreateRef<Font>(Application::GetWorkingDirectory() / "resources" / "Fonts" / "Manrope-Medium.ttf");
}

void Font::Shutdown()
{
	s_DefaultFont.reset();
}

bool Font::Load(const std::filesystem::path& filepath)
{
	if (!std::filesystem::exists(filepath))
	{
		ENGINE_ERROR("Font does not exist: {0}", filepath);
		return false;
	}

	m_TextureAtlas.reset();

	// Try Loading the atlas cache
	std::filesystem::path cachePath = filepath;
	cachePath.replace_extension(".png");
	if (std::filesystem::exists(cachePath))
	{
		m_TextureAtlas = Texture2D::Create(cachePath);
		m_TextureAtlas->SetFilterMethod(Texture::FilterMethod::Linear);
	}

	msdfgen::FreetypeHandle* ftHandle = msdfgen::initializeFreetype();
	msdfgen::FontHandle* fontHandle = msdfgen::loadFont(ftHandle, filepath.string().c_str());

	if (!ftHandle || !fontHandle)
	{
		ENGINE_ERROR("Could not load font: {0}", filepath);
		msdfgen::destroyFont(fontHandle);
		msdfgen::deinitializeFreetype(ftHandle);
		return false;
	}	

	m_Filepath = filepath;

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
	generator.setThreadCount(4);
	generator.generate(m_MSDFData->glyphs.data(), (int)m_MSDFData->glyphs.size());

	msdfgen::BitmapConstRef<float, bytes> bitmap = (msdfgen::BitmapConstRef<float, bytes>)generator.atlasStorage();

	m_TextureAtlas = Texture2D::Create(bitmap.width, bitmap.height, Texture::Format::RGBA32F, (void*)bitmap.pixels);
	m_TextureAtlas->SetFilterMethod(Texture::FilterMethod::Linear);

	msdfgen::destroyFont(fontHandle);
	msdfgen::deinitializeFreetype(ftHandle);

	// Cache the texture to png file
	msdfgen::savePng(bitmap, cachePath.string().c_str());
	return true;
}