#pragma once
#include <filesystem>

// Dumps the Lua API registry (LuaManager::GetIdentifiers - see LuaApiEntry) to Markdown,
// one page per component plus a page for global bindings and an index page linking both.
// Requires nothing beyond Application::Init() having run (which calls LuaManager::Init()
// before any window/renderer is created), so this can run fully headless - see the
// --generate-docs flag in Editor/src/main.cpp and the GenerateLuaDocs CMake target.
class LuaDocGenerator
{
public:
	static void Generate(const std::filesystem::path& outputDirectory);
};
