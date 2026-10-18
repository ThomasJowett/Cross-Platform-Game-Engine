#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

// Projects in the web Editor: copied from a folder on disk into memory, with changes written back.
// Everything here does nothing (or returns nothing) outside the web build.
class WebProject
{
public:
	// Lets the user pick a project folder, copies it into memory and returns its .proj file
	static std::optional<std::filesystem::path> OpenFolder();

	// Lets the user pick an empty folder for a new project and returns its in-memory directory
	static std::optional<std::filesystem::path> ChooseNewProjectFolder();

	// Names of recently opened project folders, most recent first
	static std::vector<std::string> GetRecentProjects();

	// Reopens a recent project folder, asking for permission if needed, and returns its .proj file
	static std::optional<std::filesystem::path> OpenRecent(const std::string& name);

	// Starts writing changes in the open projects back to disk
	static void Sync();

	// Saves settings and project changes on a timer
	static void Update(float deltaTime);

	// True when the browser can write to folders on disk (Chromium-based browsers)
	static bool CanWriteToDisk();

	// True when the project was opened without write access, so changes can only be downloaded
	static bool IsReadOnly(const std::filesystem::path& projectDirectory);

	// Zips a directory and downloads it, with its contents inside a folder of the same name or at the top of the zip
	static void DownloadAsZip(const std::filesystem::path& directory, bool includeFolder = true);

	// Hands a file in memory to the browser as a download
	static void Download(const std::filesystem::path& filepath, const std::string& downloadName);

	// Fetches a file from the server hosting the Editor into memory
	static bool FetchToFile(const std::string& url, const std::filesystem::path& filepath);
};
