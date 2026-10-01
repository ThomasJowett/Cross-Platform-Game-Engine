#include "LuaDocGenerator.h"
#include "Scripting/Lua/LuaManager.h"

#include <fstream>
#include <map>
#include <set>
#include <algorithm>
#include <iostream>

namespace
{
	void WriteEntry(std::ofstream& file, const LuaApiEntry& entry)
	{
		if (entry.kind == LuaApiEntry::Kind::Property)
			file << "- **" << entry.name << "** (`" << entry.type << "`): " << entry.description << "\n";
		else if (entry.kind == LuaApiEntry::Kind::Function || entry.kind == LuaApiEntry::Kind::ComponentAccessor)
			file << "- **" << entry.name << "()**: " << entry.description << "\n";
		else
			file << "- **" << entry.name << "**: " << entry.description << "\n";
	}

	void SortByName(std::vector<LuaApiEntry>& entries)
	{
		std::sort(entries.begin(), entries.end(), [](const LuaApiEntry& a, const LuaApiEntry& b) { return a.name < b.name; });
	}
}

void LuaDocGenerator::Generate(const std::filesystem::path& outputDirectory)
{
	std::filesystem::create_directories(outputDirectory);

	const std::vector<LuaApiEntry>& entries = LuaManager::GetIdentifiers();

	std::map<std::string, std::vector<LuaApiEntry>> byComponent;	// std::map, not unordered_map - alphabetical iteration gives stable page ordering below
	std::vector<LuaApiEntry> globalFunctions;	// bare functions with no owning table, e.g. ChangeScene()
	std::set<std::string> allComponentNames;	// every ECS component RegisterComponent<T> has run for, migrated to REFLECT_LUA_* or not

	for (const LuaApiEntry& entry : entries)
	{
		if (entry.kind == LuaApiEntry::Kind::ComponentAccessor)
		{
			allComponentNames.insert(entry.component);
			continue;
		}
		if (entry.kind == LuaApiEntry::Kind::Global)
			continue;
		if (entry.component.empty())
			globalFunctions.push_back(entry);
		else
			byComponent[entry.component].push_back(entry);
	}

	// A Kind::Global entry whose name matches a page generated above (e.g. "Log", added via
	// AddIdentifier("Log", ...) right where the Log table's own methods are registered) names
	// a globally-accessible table, not a plain value - link to its page instead of listing it
	// as a bullet. Anything left (e.g. CurrentEntity) is a genuine bare value.
	std::vector<LuaApiEntry> globalTables, globalValues;
	for (const LuaApiEntry& entry : entries)
	{
		if (entry.kind != LuaApiEntry::Kind::Global)
			continue;
		(byComponent.count(entry.name) ? globalTables : globalValues).push_back(entry);
	}

	SortByName(globalFunctions);
	SortByName(globalTables);
	SortByName(globalValues);

	{
		std::ofstream file(outputDirectory / "Globals.md");
		file << "# Globals\n\n";
		file << "Functions and values available to every Lua script without going through an entity/component.\n\n";

		if (!globalFunctions.empty())
		{
			file << "## Functions\n\n";
			for (const LuaApiEntry& entry : globalFunctions)
				WriteEntry(file, entry);
			file << "\n";
		}

		if (!globalValues.empty())
		{
			file << "## Values\n\n";
			for (const LuaApiEntry& entry : globalValues)
				WriteEntry(file, entry);
			file << "\n";
		}

		if (!globalTables.empty())
		{
			file << "## Tables\n\n";
			file << "Always-available tables, called directly (e.g. `Log.Debug(...)`) rather than through an entity.\n\n";
			for (const LuaApiEntry& entry : globalTables)
				file << "- [" << entry.name << "](" << entry.name << ".md): " << entry.description << "\n";
			file << "\n";
		}
	}

	// Hand-written component pages live under docs/Components/<category>/<Name>.md. Found by
	// filename, so the generator doesn't need to know which category each component is in.
	const std::filesystem::path outputDir = outputDirectory.lexically_normal();
	const std::filesystem::path docsDir = (outputDir / "..").lexically_normal();
	std::map<std::string, std::filesystem::path> componentPages;	// component name -> page path relative to docs/
	if (std::filesystem::exists(docsDir / "Components"))
	{
		for (const auto& dirEntry : std::filesystem::recursive_directory_iterator(docsDir / "Components"))
		{
			if (dirEntry.is_regular_file() && dirEntry.path().extension() == ".md")
				componentPages[dirEntry.path().stem().string()] = dirEntry.path().lexically_normal().lexically_relative(docsDir);
		}
	}

	// Link from a page at docsRelativeFrom (relative to docs/) to docsRelativeTo
	auto linkBetween = [](const std::filesystem::path& docsRelativeFrom, const std::filesystem::path& docsRelativeTo)
	{
		return docsRelativeTo.lexically_relative(docsRelativeFrom.parent_path()).generic_string();
	};

	for (const std::string& componentName : allComponentNames)
	{
		if (!componentPages.count(componentName))
			std::cerr << "Warning: no documentation page for " << componentName << " - add docs/Components/<category>/" << componentName << ".md" << std::endl;
	}

	for (auto& [component, componentEntries] : byComponent)
	{
		if (allComponentNames.count(component))
			continue;	// written as a fragment of the component's own page below

		std::vector<LuaApiEntry> properties, functions;
		for (const LuaApiEntry& entry : componentEntries)
			(entry.kind == LuaApiEntry::Kind::Property ? properties : functions).push_back(entry);

		SortByName(properties);
		SortByName(functions);

		std::ofstream file(outputDir / (component + ".md"));
		file << "# " << component << "\n\n";

		if (!properties.empty())
		{
			file << "## Properties\n\n";
			for (const LuaApiEntry& entry : properties)
				WriteEntry(file, entry);
			file << "\n";
		}

		if (!functions.empty())
		{
			file << "## Functions\n\n";
			for (const LuaApiEntry& entry : functions)
				WriteEntry(file, entry);
			file << "\n";
		}

		// Entity is the one page that also needs to explain and index the generic
		// Add/Get/Has/Remove/GetOrAdd pattern every component gets - see the comment on
		// LuaApiEntry::Kind::ComponentAccessor for why those aren't listed on each
		// component's own page instead.
		if (component == "Entity" && !allComponentNames.empty())
		{
			file << "## Components\n\n";
			file << "Every component below can be added to/read from any entity via `entity:Add<Name>()`, "
				"`entity:Get<Name>()`, `entity:GetOrAdd<Name>()`, `entity:Has<Name>()` and `entity:Remove<Name>()` "
				"- e.g. `entity:AddTilemapComponent()`.\n\n";
			for (const std::string& componentName : allComponentNames)
			{
				if (componentPages.count(componentName))
					file << "- [" << componentName << "](" << linkBetween("LuaAPI/Entity.md", componentPages[componentName]) << ")\n";
				else
					file << "- " << componentName << "\n";
			}
			file << "\n";
		}
	}

	// Each component's Lua section, pulled into its hand-written page with pymdownx.snippets.
	// Written for every component, bound or not, so every page can include one unconditionally.
	const std::filesystem::path fragmentsDir = outputDir / "_fragments";
	std::filesystem::remove_all(fragmentsDir);
	std::filesystem::create_directories(fragmentsDir);
	for (const std::string& componentName : allComponentNames)
	{
		std::vector<LuaApiEntry> properties, functions;
		if (auto it = byComponent.find(componentName); it != byComponent.end())
		{
			for (const LuaApiEntry& entry : it->second)
				(entry.kind == LuaApiEntry::Kind::Property ? properties : functions).push_back(entry);
		}
		SortByName(properties);
		SortByName(functions);

		std::ofstream file(fragmentsDir / (componentName + ".md"));
		file << "## Lua scripting\n\n";

		if (properties.empty() && functions.empty())
			file << "No properties or functions of this component are exposed to Lua.\n\n";

		if (!properties.empty())
		{
			file << "### Properties\n\n";
			for (const LuaApiEntry& entry : properties)
				WriteEntry(file, entry);
			file << "\n";
		}

		if (!functions.empty())
		{
			file << "### Functions\n\n";
			for (const LuaApiEntry& entry : functions)
				WriteEntry(file, entry);
			file << "\n";
		}
	}

	{
		// index.md, not Home.md - MkDocs' convention for a section's landing page.
		std::ofstream file(outputDir / "index.md");
		file << "# Lua API Reference\n\n";
		file << "- [Globals](Globals.md)\n";
		for (auto& [component, componentEntries] : byComponent)
		{
			if (!allComponentNames.count(component))
				file << "- [" << component << "](" << component << ".md)\n";
		}
		file << "\n## Components\n\n";
		file << "Each component's Lua properties and functions are listed on its own page.\n\n";
		for (const std::string& componentName : allComponentNames)
		{
			if (componentPages.count(componentName))
				file << "- [" << componentName << "](" << linkBetween("LuaAPI/index.md", componentPages[componentName]) << ")\n";
			else
				file << "- " << componentName << "\n";
		}
	}
}
