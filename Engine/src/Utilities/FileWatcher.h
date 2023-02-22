#pragma once

#include <filesystem>
#include <chrono>
#include <thread>
#include <unordered_map>
#include <string>
#include <functional>

enum class FileStatus { Created, Modified, Erased };

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
		m_PathToWatch = pathToWatch;
		if (std::filesystem::exists(pathToWatch))
		{
			for (auto& file : std::filesystem::recursive_directory_iterator(m_PathToWatch))
			{
				m_Paths[file.path().string()] = std::filesystem::last_write_time(file);
			}
		}
	}

	void RunCheck()
	{
		while (m_Running)
		{
			// wait for delay (ms)
			std::this_thread::sleep_for(m_Delay);

			if (!std::filesystem::exists(m_PathToWatch))
				continue;

			auto it = m_Paths.begin();
			while (it != m_Paths.end())
			{
				// Check if file has been erased
				if (!std::filesystem::exists(it->first))
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
			for (const std::filesystem::directory_entry& file : std::filesystem::recursive_directory_iterator(m_PathToWatch))
			{
				auto lastWriteTime = std::filesystem::last_write_time(file);

				// File Created
				if (!Contains(file.path().string()))
				{
					m_Paths[file.path().string()] = lastWriteTime;
					m_Callback(file.path().string(), FileStatus::Created);
				}
				// File modified
				else
				{
					if (m_Paths[file.path().string()] != lastWriteTime)
					{
						m_Paths[file.path().string()] = lastWriteTime;
						m_Callback(file.path().string(), FileStatus::Modified);
					}
				}
			}
		}
	}

	void Start(const std::function<void(std::string, FileStatus)> callback)
	{
		m_Callback = std::bind(callback, std::placeholders::_1, std::placeholders::_2);
		m_CheckThread = std::thread(&FileWatcher::RunCheck, this);
	}

	void Stop()
	{
		m_Running = false;
		m_CheckThread.join();
	}
	
private:
	bool Contains(const std::string& key)
	{
		auto el = m_Paths.find(key);
		return el != m_Paths.end();
	}
	std::filesystem::path m_PathToWatch;

	std::chrono::duration<int, std::milli> m_Delay;

	std::unordered_map<std::string, std::filesystem::file_time_type> m_Paths;

	bool m_Running = true;

	std::thread m_CheckThread;

	std::function<void(std::string, FileStatus)> m_Callback;
};
