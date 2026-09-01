#include "LuaDocGenerator.h"
#include "Scripting/Lua/LuaManager.h"

#include <fstream>
#include <map>
#include <set>
#include <algorithm>

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
	std::filesystem::create_directories(outputDirectory / "Components");

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

	// Every entry for a given name comes from the same registration path (RegisterComponent<T>
	// for a real ECS component, or a hand-written Bind* for a utility table), so isComponent is
	// consistent within a group - safe to just read it off the first entry.
	auto relativePathFor = [](const std::string& name, bool isComponent)
	{
		return isComponent ? ("Components/" + name + ".md") : (name + ".md");
	};

	for (auto& [component, componentEntries] : byComponent)
	{
		std::vector<LuaApiEntry> properties, functions;
		for (const LuaApiEntry& entry : componentEntries)
			(entry.kind == LuaApiEntry::Kind::Property ? properties : functions).push_back(entry);

		SortByName(properties);
		SortByName(functions);

		std::ofstream file(outputDirectory / relativePathFor(component, componentEntries.front().isComponent));
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
				if (byComponent.count(componentName))
					file << "- [" << componentName << "](Components/" << componentName << ".md)\n";
				else
					file << "- " << componentName << "\n";
			}
			file << "\n";
		}
	}

	{
		// index.md, not Home.md - MkDocs' convention for a section's landing page.
		std::ofstream file(outputDirectory / "index.md");
		file << "# Lua API Reference\n\n";
		file << "- [Globals](Globals.md)\n";
		for (auto& [component, componentEntries] : byComponent)
			file << "- [" << component << "](" << relativePathFor(component, componentEntries.front().isComponent) << ")\n";
	}
}
