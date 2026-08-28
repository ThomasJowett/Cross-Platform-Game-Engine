#include "SpriteAtlas.h"

#include "TinyXml2/tinyxml2.h"
#include "Core/Version.h"
#include "Scene/AssetManager.h"
#include "Logging/Instrumentor.h"

#include <stb/stb_image.h>
#include <stb/stb_image_write.h>
#include <stb/stb_rect_pack.h>

namespace
{
	constexpr int PADDING = 2;

	struct DecodedImage
	{
		std::filesystem::path path;
		int width = 0;
		int height = 0;
		std::vector<uint8_t> pixels;
		int64_t mtime = 0;
		uintmax_t size = 0;
	};

	void ExtrudeBorder(std::vector<uint8_t>& page, int pageSize, int innerX, int innerY, int innerW, int innerH)
	{
		auto pixelAt = [&](int x, int y) -> uint8_t* { return &page[((size_t)y * pageSize + x) * 4]; };

		for (int y = innerY; y < innerY + innerH; y++)
		{
			uint8_t* left = pixelAt(innerX, y);
			uint8_t* right = pixelAt(innerX + innerW - 1, y);
			for (int p = 1; p <= PADDING; p++)
			{
				std::memcpy(pixelAt(innerX - p, y), left, 4);
				std::memcpy(pixelAt(innerX + innerW - 1 + p, y), right, 4);
			}
		}

		int paddedX = innerX - PADDING;
		int paddedW = innerW + PADDING * 2;
		for (int p = 1; p <= PADDING; p++)
		{
			std::memcpy(pixelAt(paddedX, innerY - p), pixelAt(paddedX, innerY), (size_t)paddedW * 4);
			std::memcpy(pixelAt(paddedX, innerY + innerH - 1 + p), pixelAt(paddedX, innerY + innerH - 1), (size_t)paddedW * 4);
		}
	}
}

SpriteAtlas::SpriteAtlas(const std::filesystem::path& filepath)
{
	Load(filepath);
}

SpriteAtlas::SpriteAtlas(const std::filesystem::path& filepath, const std::vector<uint8_t>& data)
{
	Load(filepath, data);
}

bool SpriteAtlas::Load(const std::filesystem::path& filepath)
{
	PROFILE_FUNCTION();
	std::filesystem::path absolutePath = std::filesystem::absolute(Application::GetOpenDocumentDirectory() / filepath);
	if (!std::filesystem::exists(absolutePath))
		return false;

	tinyxml2::XMLDocument doc;
	if (doc.LoadFile(absolutePath.string().c_str()) != tinyxml2::XML_SUCCESS)
	{
		ENGINE_ERROR("Could not load sprite atlas manifest {0}. {1} on line {2}", absolutePath, doc.ErrorName(), doc.ErrorLineNum());
		return false;
	}

	if (!LoadXML(&doc, filepath.parent_path()))
	{
		ENGINE_ERROR("Could not load sprite atlas manifest: {0}", absolutePath);
		return false;
	}

	m_Filepath = filepath;
	m_Filepath.make_preferred();
	return true;
}

bool SpriteAtlas::Load(const std::filesystem::path& filepath, const std::vector<uint8_t>& data)
{
	PROFILE_FUNCTION();
	tinyxml2::XMLDocument doc;
	if (doc.Parse((const char*)data.data(), data.size()) != tinyxml2::XML_SUCCESS)
	{
		ENGINE_ERROR("Could not load sprite atlas manifest from memory {0}. {1} on line {2}", filepath, doc.ErrorName(), doc.ErrorLineNum());
		return false;
	}

	if (!LoadXML(&doc, filepath.parent_path()))
	{
		ENGINE_ERROR("Could not load sprite atlas manifest from memory: {0}", filepath);
		return false;
	}

	m_Filepath = filepath;
	m_Filepath.make_preferred();
	return true;
}

bool SpriteAtlas::LoadXML(tinyxml2::XMLDocument* doc, const std::filesystem::path& manifestDir)
{
	tinyxml2::XMLElement* pRoot = doc->FirstChildElement("SpriteAtlas");
	if (!pRoot)
	{
		ENGINE_ERROR("Sprite atlas manifest does not contain a SpriteAtlas node");
		return false;
	}

	if (const char* version = pRoot->Attribute("EngineVersion"); version && atoi(version) != VERSION)
		ENGINE_WARN("Sprite atlas manifest created with a different version of the engine");

	uint32_t pageSize = 0;
	pRoot->QueryUnsignedAttribute("PageSize", &pageSize);

	m_PagePaths.clear();
	m_PageCache.clear();
	m_Sources.clear();
	m_Regions.clear();

	tinyxml2::XMLElement* pPage = pRoot->FirstChildElement("Page");
	while (pPage)
	{
		uint32_t index = 0;
		pPage->QueryUnsignedAttribute("Index", &index);
		if (const char* file = pPage->Attribute("File"))
		{
			if (m_PagePaths.size() <= index)
				m_PagePaths.resize(index + 1);
			m_PagePaths[index] = manifestDir / file;
		}
		pPage = pPage->NextSiblingElement("Page");
	}
	m_PageCache.resize(m_PagePaths.size());

	tinyxml2::XMLElement* pRegion = pRoot->FirstChildElement("Region");
	while (pRegion)
	{
		const char* path = pRegion->Attribute("Path");
		uint32_t page = 0;
		int x = 0, y = 0, w = 0, h = 0;
		int64_t mtime = 0;
		int64_t size = 0;
		pRegion->QueryUnsignedAttribute("Page", &page);
		pRegion->QueryIntAttribute("X", &x);
		pRegion->QueryIntAttribute("Y", &y);
		pRegion->QueryIntAttribute("W", &w);
		pRegion->QueryIntAttribute("H", &h);
		pRegion->QueryInt64Attribute("MTime", &mtime);
		pRegion->QueryInt64Attribute("Size", &size);

		if (path && page < m_PagePaths.size() && !m_PagePaths[page].empty() && pageSize > 0)
		{
			m_Sources.push_back({ path, mtime, (uintmax_t)size });

			Region region;
			region.page = page;
			region.uvMin = { (float)x / pageSize, (float)y / pageSize };
			region.uvMax = { (float)(x + w) / pageSize, (float)(y + h) / pageSize };
			m_Regions[path] = region;
		}

		pRegion = pRegion->NextSiblingElement("Region");
	}

	return true;
}

const SpriteAtlas::Region* SpriteAtlas::GetRegion(const std::filesystem::path& path) const
{
	auto it = m_Regions.find(path);
	return it != m_Regions.end() ? &it->second : nullptr;
}

Ref<Texture2D> SpriteAtlas::GetPage(uint32_t index) const
{
	if (index >= m_PagePaths.size())
		return nullptr;

	if (!m_PageCache[index])
		m_PageCache[index] = AssetManager::GetTexture(m_PagePaths[index]);

	return m_PageCache[index];
}

Ref<SpriteAtlas> SpriteAtlas::Build(const std::vector<std::filesystem::path>& sourceImagePaths, const std::filesystem::path& outputDir, uint32_t pageSize)
{
	PROFILE_FUNCTION();

	std::filesystem::path absoluteOutputDir = std::filesystem::absolute(Application::GetOpenDocumentDirectory() / outputDir);
	std::error_code ec;
	std::filesystem::create_directories(absoluteOutputDir, ec);

	std::vector<DecodedImage> images;
	images.reserve(sourceImagePaths.size());
	for (const std::filesystem::path& relativePath : sourceImagePaths)
	{
		std::filesystem::path absolutePath = std::filesystem::absolute(Application::GetOpenDocumentDirectory() / relativePath);
		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* pixels = stbi_load(absolutePath.string().c_str(), &width, &height, &channels, 4);
		if (!pixels)
		{
			ENGINE_ERROR("Sprite atlas: failed to load source image {0}: {1}", absolutePath, stbi_failure_reason());
			continue;
		}

		DecodedImage image;
		image.path = relativePath;
		image.width = width;
		image.height = height;
		image.pixels.assign(pixels, pixels + (size_t)width * height * 4);
		stbi_image_free(pixels);

		std::error_code statEc;
		image.mtime = std::filesystem::last_write_time(absolutePath, statEc).time_since_epoch().count();
		image.size = std::filesystem::file_size(absolutePath, statEc);

		images.push_back(std::move(image));
	}

	if (images.empty())
		return nullptr;

	std::vector<stbrp_rect> rects(images.size());
	for (size_t i = 0; i < images.size(); i++)
	{
		rects[i].id = (int)i;
		rects[i].w = (stbrp_coord)(images[i].width + PADDING * 2);
		rects[i].h = (stbrp_coord)(images[i].height + PADDING * 2);
	}

	struct PlacedRect { size_t imageIndex; uint32_t page; int x, y; };
	std::vector<PlacedRect> placed;
	std::vector<std::vector<uint8_t>> pageBuffers;

	std::vector<stbrp_rect> remaining = rects;
	uint32_t pageIndex = 0;
	while (!remaining.empty())
	{
		std::vector<stbrp_node> nodes(pageSize);
		stbrp_context context;
		stbrp_init_target(&context, (int)pageSize, (int)pageSize, nodes.data(), (int)nodes.size());
		stbrp_pack_rects(&context, remaining.data(), (int)remaining.size());

		pageBuffers.emplace_back((size_t)pageSize * pageSize * 4, 0);

		std::vector<stbrp_rect> stillRemaining;
		for (const stbrp_rect& rect : remaining)
		{
			if (rect.was_packed)
				placed.push_back({ (size_t)rect.id, pageIndex, rect.x, rect.y });
			else
				stillRemaining.push_back(rect);
		}

		if (stillRemaining.size() == remaining.size())
		{
			ENGINE_ERROR("Sprite atlas: one or more source images are too large to fit a {0}x{0} page", pageSize);
			break;
		}

		remaining = std::move(stillRemaining);
		pageIndex++;
	}

	for (const PlacedRect& p : placed)
	{
		const DecodedImage& image = images[p.imageIndex];
		std::vector<uint8_t>& page = pageBuffers[p.page];
		int innerX = p.x + PADDING;
		int innerY = p.y + PADDING;

		for (int row = 0; row < image.height; row++)
		{
			uint8_t* dst = &page[((size_t)(innerY + row) * pageSize + innerX) * 4];
			const uint8_t* src = &image.pixels[(size_t)row * image.width * 4];
			std::memcpy(dst, src, (size_t)image.width * 4);
		}

		ExtrudeBorder(page, (int)pageSize, innerX, innerY, image.width, image.height);
	}

	std::vector<std::pair<size_t, PlacedRect>> manifestRegions;
	for (const PlacedRect& p : placed)
		manifestRegions.push_back({ p.imageIndex, p });

	std::filesystem::path manifestPath = outputDir / "Manifest.atlas";
	std::filesystem::path absoluteManifestPath = absoluteOutputDir / "Manifest.atlas";

	tinyxml2::XMLDocument doc;
	tinyxml2::XMLElement* pRoot = doc.NewElement("SpriteAtlas");
	pRoot->SetAttribute("EngineVersion", VERSION);
	pRoot->SetAttribute("PageSize", pageSize);
	doc.InsertFirstChild(pRoot);

	for (uint32_t page = 0; page < pageBuffers.size(); page++)
	{
		std::vector<uint8_t>& buffer = pageBuffers[page];
		std::vector<uint8_t> flipped(buffer.size());
		size_t rowBytes = (size_t)pageSize * 4;
		for (uint32_t row = 0; row < pageSize; row++)
			std::memcpy(&flipped[(size_t)row * rowBytes], &buffer[(size_t)(pageSize - 1 - row) * rowBytes], rowBytes);

		std::string pageFileName = "Page" + std::to_string(page) + ".png";
		std::filesystem::path absolutePagePath = absoluteOutputDir / pageFileName;
		stbi_write_png(absolutePagePath.string().c_str(), (int)pageSize, (int)pageSize, 4, flipped.data(), (int)rowBytes);

		tinyxml2::XMLElement* pPage = pRoot->InsertNewChildElement("Page");
		pPage->SetAttribute("Index", page);
		pPage->SetAttribute("File", pageFileName.c_str());
	}

	for (const auto& [imageIndex, p] : manifestRegions)
	{
		const DecodedImage& image = images[imageIndex];
		std::string pathString = image.path.string();
		std::replace(pathString.begin(), pathString.end(), '\\', '/');

		tinyxml2::XMLElement* pRegion = pRoot->InsertNewChildElement("Region");
		pRegion->SetAttribute("Path", pathString.c_str());
		pRegion->SetAttribute("Page", p.page);
		pRegion->SetAttribute("X", p.x + PADDING);
		pRegion->SetAttribute("Y", p.y + PADDING);
		pRegion->SetAttribute("W", image.width);
		pRegion->SetAttribute("H", image.height);
		pRegion->SetAttribute("MTime", (int64_t)image.mtime);
		pRegion->SetAttribute("Size", (int64_t)image.size);
	}

	if (doc.SaveFile(absoluteManifestPath.string().c_str()) != tinyxml2::XML_SUCCESS)
	{
		ENGINE_ERROR("Sprite atlas: failed to write manifest {0}", absoluteManifestPath);
		return nullptr;
	}

	Ref<SpriteAtlas> atlas = CreateRef<SpriteAtlas>();
	if (!atlas->Load(manifestPath))
		return nullptr;
	return atlas;
}
