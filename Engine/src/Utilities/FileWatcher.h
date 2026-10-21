#pragma once

#include <filesystem>
#include <chrono>
#include <thread>
#include <unordered_map>
#include <string>
#include <functional>
#include <mutex>
#include <atomic>

enum class FileStatus { Created, Modified, Erased };

#ifndef __EMSCRIPTEN__

class FileWatcher
{
public:
	FileWatcher(const FileWatcher&) = delete;
	FileWatcher(std::chrono::duration<int, std::milli> delay)
		:m_Delay(delay)
	{
	}

	~FileWatcher()
	{
		Stop();
	}

	void SetPathToWatch(const std::filesystem::path& pathToWatch)
	{
		std::lock_guard<std::mutex> lock(m_Mutex);
		m_PathToWatch = pathToWatch;
		ForEachFile([this](const std::filesystem::path& path, std::filesystem::file_time_type lastWriteTime)
			{
				m_Paths[path.string()] = lastWriteTime;
			});
	}

	void RunCheck()
	{
		while (m_Running)
		{
			// wait for delay (ms)
			std::this_thread::sleep_for(m_Delay);

			std::lock_guard<std::mutex> lock(m_Mutex);

			std::error_code error;
			if (!std::filesystem::exists(m_PathToWatch, error))
				continue;

			auto it = m_Paths.begin();
			while (it != m_Paths.end())
			{
				// Check if file has been erased
				if (!std::filesystem::exists(it->first, error) && !error)
				{
					m_Callback(it->first, FileStatus::Erased);
					it = m_Paths.erase(it);
				}
				else
				{
					it++;
				}
			}

			// Check if a file was created or modified
			ForEachFile([this](const std::filesystem::path& path, std::filesystem::file_time_type lastWriteTime)
				{
					// File Created
					if (!Contains(path.string()))
					{
						m_Paths[path.string()] = lastWriteTime;
						m_Callback(path.string(), FileStatus::Created);
					}
					// File modified
					else if (m_Paths[path.string()] != lastWriteTime)
					{
						m_Paths[path.string()] = lastWriteTime;
						m_Callback(path.string(), FileStatus::Modified);
					}
				});
		}
	}

	void Start(const std::function<void(std::string, FileStatus)> callback)
	{
		m_Running = true;
		m_Callback = std::bind(callback, std::placeholders::_1, std::placeholders::_2);
		m_CheckThread = std::thread(&FileWatcher::RunCheck, this);
	}

	void Stop()
	{
		m_Running = false;
		if (m_CheckThread.joinable())
			m_CheckThread.join();
	}
	
private:
	// Files can move or vanish mid-scan, so skip any that error; the next scan picks them up
	template<typename Func>
	void ForEachFile(Func func)
	{
		std::error_code error;
		std::filesystem::recursive_directory_iterator end;
		for (std::filesystem::recursive_directory_iterator it(m_PathToWatch, std::filesystem::directory_options::skip_permission_denied, error);
			!error && it != end; it.increment(error))
		{
			std::filesystem::file_time_type lastWriteTime = std::filesystem::last_write_time(it->path(), error);
			if (error)
			{
				error.clear();
				continue;
			}
			func(it->path(), lastWriteTime);
		}
	}

	bool Contains(const std::string& key)
	{
		auto el = m_Paths.find(key);
		return el != m_Paths.end();
	}
	std::mutex m_Mutex;
	std::filesystem::path m_PathToWatch;

	std::chrono::duration<int, std::milli> m_Delay;

	std::unordered_map<std::string, std::filesystem::file_time_type> m_Paths;

	std::atomic<bool> m_Running = true;

	std::thread m_CheckThread;

	std::function<void(std::string, FileStatus)> m_Callback;
};
#else
// ---------------------------------------------------------
// WEB (EMSCRIPTEN) STUB IMPLEMENTATION
// ---------------------------------------------------------
class FileWatcher
{
public:
	FileWatcher(const FileWatcher&) = delete;
	FileWatcher(std::chrono::duration<int, std::milli> delay) {}
	~FileWatcher() {}

	void SetPathToWatch(const std::filesystem::path& pathToWatch) {}

	void Start(const std::function<void(std::string, FileStatus)> callback)
	{
		// On Web, we do nothing. The thread is never started.
	}

	void Stop() {}
};
#endif