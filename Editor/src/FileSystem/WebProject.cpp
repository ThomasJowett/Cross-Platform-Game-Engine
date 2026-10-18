#include "WebProject.h"

#include "Core/Settings.h"
#include "Logging/Logger.h"
#include "Utilities/StringUtils.h"

#include <miniz.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>

// The JS side lives in Resources/Web/editor_pre.js
EM_ASYNC_JS(char*, WebProject_OpenFolder, (), {
	return AttercopWeb.toC(await AttercopWeb.openProjectFolder());
});

EM_ASYNC_JS(char*, WebProject_ChooseNewProjectFolder, (), {
	return AttercopWeb.toC(await AttercopWeb.chooseNewProjectFolder());
});

EM_ASYNC_JS(char*, WebProject_GetRecentProjects, (), {
	return AttercopWeb.toC((await AttercopWeb.recentProjects()).join('\n'));
});

EM_ASYNC_JS(char*, WebProject_OpenRecent, (const char* name), {
	return AttercopWeb.toC(await AttercopWeb.openRecent(UTF8ToString(name)));
});

EM_ASYNC_JS(int, WebProject_FetchToFile, (const char* url, const char* path), {
	return await AttercopWeb.fetchToFile(UTF8ToString(url), UTF8ToString(path)) ? 1 : 0;
});

EM_JS(void, WebProject_Sync, (), {
	AttercopWeb.sync();
});

EM_JS(void, WebProject_Persist, (), {
	AttercopWeb.persist();
});

EM_JS(int, WebProject_CanWriteToDisk, (), {
	return AttercopWeb.canWriteToDisk() ? 1 : 0;
});

EM_JS(int, WebProject_IsReadOnly, (const char* root), {
	return AttercopWeb.isReadOnly(UTF8ToString(root)) ? 1 : 0;
});

EM_JS(void, WebProject_Download, (const char* path, const char* name), {
	AttercopWeb.download(UTF8ToString(path), UTF8ToString(name));
});

static std::string TakeString(char* str)
{
	std::string result = str ? str : "";
	free(str);
	return result;
}

static std::optional<std::filesystem::path> NonEmptyPath(const std::string& path)
{
	if (path.empty())
		return std::nullopt;
	return std::filesystem::path(path);
}

std::optional<std::filesystem::path> WebProject::OpenFolder()
{
	return NonEmptyPath(TakeString(WebProject_OpenFolder()));
}

std::optional<std::filesystem::path> WebProject::ChooseNewProjectFolder()
{
	return NonEmptyPath(TakeString(WebProject_ChooseNewProjectFolder()));
}

std::vector<std::string> WebProject::GetRecentProjects()
{
	std::string names = TakeString(WebProject_GetRecentProjects());
	if (names.empty())
		return {};
	return SplitString(names, '\n');
}

std::optional<std::filesystem::path> WebProject::OpenRecent(const std::string& name)
{
	return NonEmptyPath(TakeString(WebProject_OpenRecent(name.c_str())));
}

void WebProject::Sync()
{
	WebProject_Sync();
}

void WebProject::Update(float deltaTime)
{
	constexpr float syncInterval = 2.0f;
	static float timeSinceSync = 0.0f;

	timeSinceSync += deltaTime;
	if (timeSinceSync < syncInterval)
		return;
	timeSinceSync = 0.0f;

	Settings::SaveSettings();
	WebProject_Persist();
	WebProject_Sync();
}

bool WebProject::CanWriteToDisk()
{
	return WebProject_CanWriteToDisk() != 0;
}

bool WebProject::IsReadOnly(const std::filesystem::path& projectDirectory)
{
	return WebProject_IsReadOnly(projectDirectory.lexically_normal().generic_string().c_str()) != 0;
}

void WebProject::Download(const std::filesystem::path& filepath, const std::string& downloadName)
{
	WebProject_Download(filepath.string().c_str(), downloadName.c_str());
}

bool WebProject::FetchToFile(const std::string& url, const std::filesystem::path& filepath)
{
	return WebProject_FetchToFile(url.c_str(), filepath.string().c_str()) != 0;
}

#else

std::optional<std::filesystem::path> WebProject::OpenFolder() { return std::nullopt; }
std::optional<std::filesystem::path> WebProject::ChooseNewProjectFolder() { return std::nullopt; }
std::vector<std::string> WebProject::GetRecentProjects() { return {}; }
std::optional<std::filesystem::path> WebProject::OpenRecent(const std::string&) { return std::nullopt; }
void WebProject::Sync() {}
void WebProject::Update(float) {}
bool WebProject::CanWriteToDisk() { return true; }
bool WebProject::IsReadOnly(const std::filesystem::path&) { return false; }
void WebProject::Download(const std::filesystem::path&, const std::string&) {}
bool WebProject::FetchToFile(const std::string&, const std::filesystem::path&) { return false; }

#endif

void WebProject::DownloadAsZip(const std::filesystem::path& sourceDirectory, bool includeFolder)
{
	std::filesystem::path directory = sourceDirectory.lexically_normal();
	if (!directory.has_filename())
		directory = directory.parent_path();

	std::string zipName = directory.filename().string() + ".zip";
	std::filesystem::path zipPath = std::filesystem::temp_directory_path() / zipName;

	mz_zip_archive zip = {};
	if (!mz_zip_writer_init_file(&zip, zipPath.string().c_str(), 0))
	{
		ENGINE_ERROR("Could not create {0}: {1}", zipPath, mz_zip_get_error_string(mz_zip_get_last_error(&zip)));
		return;
	}

	bool ok = true;
	for (const std::filesystem::directory_entry& entry : std::filesystem::recursive_directory_iterator(directory))
	{
		if (!entry.is_regular_file())
			continue;
		std::filesystem::path relativePath = std::filesystem::relative(entry.path(), directory);
		if (includeFolder)
			relativePath = directory.filename() / relativePath;
		std::string entryName = relativePath.generic_string();
		if (!mz_zip_writer_add_file(&zip, entryName.c_str(), entry.path().string().c_str(), nullptr, 0, MZ_BEST_SPEED))
		{
			ENGINE_ERROR("Could not add {0} to {1}: {2}", entryName, zipName, mz_zip_get_error_string(mz_zip_get_last_error(&zip)));
			ok = false;
			break;
		}
	}

	ok = ok && mz_zip_writer_finalize_archive(&zip);
	mz_zip_writer_end(&zip);

	if (ok)
		Download(zipPath, zipName);

	std::error_code ec;
	std::filesystem::remove(zipPath, ec);
}
