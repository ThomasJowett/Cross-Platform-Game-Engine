#pragma once

#include <condition_variable>
#include <deque>
#include <filesystem>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "Core/core.h"
#include "Asset/Texture.h"

// Loads downscaled image thumbnails off the main thread (one per frame on web)
class ThumbnailCache
{
public:
	struct Thumbnail
	{
		Ref<Texture2D> texture;
		uint32_t sourceWidth = 0;
		uint32_t sourceHeight = 0;
		std::filesystem::file_time_type lastWriteTime;
	};

	explicit ThumbnailCache(uint32_t maxSize);
	~ThumbnailCache();

	// Returns the thumbnail if loaded, otherwise queues it and returns nullptr
	const Thumbnail* Get(const std::filesystem::path& filepath);

	// Uploads decoded thumbnails, call once per frame on the main thread
	void Update();

	// Drops queued requests that haven't started decoding yet
	void CancelPending();

	// Drops thumbnails whose file has changed or been removed
	void RemoveStale();

	void Clear();
	size_t Size() const { return m_Thumbnails.size(); }

private:
	struct Decoded
	{
		std::filesystem::path filepath;
		std::vector<uint8_t> pixels;
		uint32_t width = 0, height = 0;
		uint32_t sourceWidth = 0, sourceHeight = 0;
		std::filesystem::file_time_type lastWriteTime;
	};

	Decoded Decode(const std::filesystem::path& filepath) const;
	void WorkerLoop();

	uint32_t m_MaxSize;

	std::unordered_map<std::filesystem::path, Thumbnail> m_Thumbnails;
	std::unordered_set<std::filesystem::path> m_Requested;
	std::unordered_set<std::filesystem::path> m_Failed;

	std::mutex m_Mutex;
	std::condition_variable m_Condition;
	std::deque<std::filesystem::path> m_Queue;
	std::vector<Decoded> m_Decoded;
	std::vector<std::thread> m_Workers;
	bool m_Stopping = false;
};
