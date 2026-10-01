#pragma once
#include <filesystem>

// Dumps the Lua API registry (LuaManager::GetIdentifiers - see LuaApiEntry) to Markdown:
// a page per global table, an index page, and a "## Lua scripting" fragment per component
// under _fragments/ that the hand-written docs/Components/ pages include.
// Requires nothing beyond Application::Init() having run (which calls LuaManager::Init()
// before any window/renderer is created), so this can run fully headless - see the
// --generate-docs flag in Editor/src/main.cpp and the GenerateLuaDocs CMake target.
class LuaDocGenerator
{
public:
	static void Generate(const std::filesystem::path& outputDirectory);
};
