#include "ThumbnailCache.h"

#include <algorithm>
#include <system_error>

#include "stb_image.h"
#include "Logging/Logger.h"

ThumbnailCache::ThumbnailCache(uint32_t maxSize)
	:m_MaxSize(maxSize)
{
#ifndef __EMSCRIPTEN__
	unsigned int numWorkers = std::clamp(std::thread::hardware_concurrency() / 2, 1u, 4u);
	for (unsigned int i = 0; i < numWorkers; i++)
		m_Workers.emplace_back(&ThumbnailCache::WorkerLoop, this);
#endif
}

ThumbnailCache::~ThumbnailCache()
{
	{
		std::lock_guard<std::mutex> lock(m_Mutex);
		m_Stopping = true;
		m_Queue.clear();
	}
	m_Condition.notify_all();

	for (std::thread& worker : m_Workers)
		worker.join();
}

/* ------------------------------------------------------------------------------------------------------------------ */

const ThumbnailCache::Thumbnail* ThumbnailCache::Get(const std::filesystem::path& filepath)
{
	auto it = m_Thumbnails.find(filepath);
	if (it != m_Thumbnails.end())
		return &it->second;

	if (m_Failed.count(filepath) || !m_Requested.insert(filepath).second)
		return nullptr;

	{
		std::lock_guard<std::mutex> lock(m_Mutex);
		m_Queue.push_back(filepath);
	}
	m_Condition.notify_one();
	return nullptr;
}

/* ------------------------------------------------------------------------------------------------------------------ */

void ThumbnailCache::Update()
{
	std::vector<Decoded> decoded;

#ifdef __EMSCRIPTEN__
	// No threads in the web build
	if (!m_Queue.empty())
	{
		decoded.push_back(Decode(m_Queue.front()));
		m_Queue.pop_front();
	}
#else
	{
		std::lock_guard<std::mutex> lock(m_Mutex);
		decoded.swap(m_Decoded);
	}
#endif

	for (Decoded& result : decoded)
	{
		// Cancelled or cleared while decoding
		if (!m_Requested.erase(result.filepath))
			continue;

		if (result.pixels.empty())
		{
			m_Failed.insert(result.filepath);
			continue;
		}

		Thumbnail thumbnail;
		thumbnail.texture = Texture2D::Create(result.width, result.height, Texture::Format::RGBA, 1, result.pixels.data());
		thumbnail.sourceWidth = result.sourceWidth;
		thumbnail.sourceHeight = result.sourceHeight;
		thumbnail.lastWriteTime = result.lastWriteTime;
		m_Thumbnails[result.filepath] = thumbnail;
	}
}

/* ------------------------------------------------------------------------------------------------------------------ */

void ThumbnailCache::CancelPending()
{
	std::lock_guard<std::mutex> lock(m_Mutex);
	for (const std::filesystem::path& filepath : m_Queue)
		m_Requested.erase(filepath);
	m_Queue.clear();
}

/* ------------------------------------------------------------------------------------------------------------------ */

void ThumbnailCache::RemoveStale()
{
	for (auto it = m_Thumbnails.begin(); it != m_Thumbnails.end();)
	{
		std::error_code error;
		std::filesystem::file_time_type lastWriteTime = std::filesystem::last_write_time(it->first, error);
		if (error || lastWriteTime != it->second.lastWriteTime)
			it = m_Thumbnails.erase(it);
		else
			++it;
	}
	m_Failed.clear();
}

/* ------------------------------------------------------------------------------------------------------------------ */

void ThumbnailCache::Clear()
{
	CancelPending();
	m_Thumbnails.clear();
	m_Failed.clear();
	// In-flight decodes are discarded by Update as they are no longer requested
	m_Requested.clear();
}

/* ------------------------------------------------------------------------------------------------------------------ */

ThumbnailCache::Decoded ThumbnailCache::Decode(const std::filesystem::path& filepath) const
{
	Decoded result;
	result.filepath = filepath;

	std::error_code error;
	result.lastWriteTime = std::filesystem::last_write_time(filepath, error);

	// Matches the orientation of textures loaded by the renderer
	stbi_set_flip_vertically_on_load_thread(1);

	int width, height, channels;
	stbi_uc* data = stbi_load(filepath.string().c_str(), &width, &height, &channels, STBI_rgb_alpha);
	if (!data)
	{
		ENGINE_WARN("Failed to load thumbnail for {0}: {1}", filepath.string(), stbi_failure_reason());
		return result;
	}

	result.sourceWidth = (uint32_t)width;
	result.sourceHeight = (uint32_t)height;

	float scale = std::min(1.0f, (float)m_MaxSize / (float)std::max(width, height));
	result.width = std::max(1u, (uint32_t)(width * scale));
	result.height = std::max(1u, (uint32_t)(height * scale));
	result.pixels.resize((size_t)result.width * result.height * 4);

	// Box filter: average every source pixel that falls in each destination pixel
	for (uint32_t y = 0; y < result.height; y++)
	{
		uint32_t srcY0 = y * height / result.height;
		uint32_t srcY1 = std::max(srcY0 + 1, (y + 1) * height / result.height);
		for (uint32_t x = 0; x < result.width; x++)
		{
			uint32_t srcX0 = x * width / result.width;
			uint32_t srcX1 = std::max(srcX0 + 1, (x + 1) * width / result.width);

			uint32_t sum[4] = { 0, 0, 0, 0 };
			for (uint32_t sy = srcY0; sy < srcY1; sy++)
			{
				const stbi_uc* row = data + ((size_t)sy * width + srcX0) * 4;
				for (uint32_t sx = srcX0; sx < srcX1; sx++, row += 4)
				{
					sum[0] += row[0];
					sum[1] += row[1];
					sum[2] += row[2];
					sum[3] += row[3];
				}
			}

			uint32_t count = (srcY1 - srcY0) * (srcX1 - srcX0);
			uint8_t* dst = &result.pixels[((size_t)y * result.width + x) * 4];
			for (int c = 0; c < 4; c++)
				dst[c] = (uint8_t)(sum[c] / count);
		}
	}

	stbi_image_free(data);
	return result;
}

/* ------------------------------------------------------------------------------------------------------------------ */

void ThumbnailCache::WorkerLoop()
{
	while (true)
	{
		std::filesystem::path filepath;
		{
			std::unique_lock<std::mutex> lock(m_Mutex);
			m_Condition.wait(lock, [this]() { return m_Stopping || !m_Queue.empty(); });
			if (m_Stopping)
				return;
			filepath = m_Queue.front();
			m_Queue.pop_front();
		}

		Decoded result = Decode(filepath);

		std::lock_guard<std::mutex> lock(m_Mutex);
		m_Decoded.push_back(std::move(result));
	}
}
