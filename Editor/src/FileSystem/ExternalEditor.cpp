#include "ExternalEditor.h"

#include "Logging/Logger.h"
#include "IconsFontAwesome6.h"
#include "imgui.h"

void ExternalEditor::MenuItem(const std::filesystem::path& filepath)
{
	if (IsSupported() && ImGui::MenuItem(ICON_FA_UP_RIGHT_FROM_SQUARE" Open Externally"))
		OpenFile(filepath);
}

#ifdef _WINDOWS
#include <Windows.h>
#include <shellapi.h>

bool ExternalEditor::OpenFile(const std::filesystem::path& filepath)
{
	HINSTANCE result = ShellExecuteW(nullptr, L"open", filepath.wstring().c_str(), nullptr, nullptr, SW_SHOWNORMAL);
	if ((INT_PTR)result <= 32)
	{
		CLIENT_ERROR("Failed to open {0} in external editor", filepath.string());
		return false;
	}
	return true;
}

#elif defined(__EMSCRIPTEN__)

bool ExternalEditor::OpenFile(const std::filesystem::path& filepath)
{
	CLIENT_WARN("Opening files in an external editor is not supported on web");
	return false;
}

#else
#include <spawn.h>
#include <sys/wait.h>
#include <thread>

extern char** environ;

bool ExternalEditor::OpenFile(const std::filesystem::path& filepath)
{
#ifdef __APPLE__
	const char* launcher = "open";
#else
	const char* launcher = "xdg-open";
#endif

	std::string path = filepath.string();
	char* argv[] = { const_cast<char*>(launcher), path.data(), nullptr };

	pid_t pid;
	if (posix_spawnp(&pid, launcher, nullptr, nullptr, argv, environ) != 0)
	{
		CLIENT_ERROR("Failed to open {0} in external editor", filepath.string());
		return false;
	}

	// Reap the launcher without blocking the editor
	std::thread([pid]() { int status; waitpid(pid, &status, 0); }).detach();
	return true;
}
#endif
