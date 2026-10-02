#pragma once

#include <array>
#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

// Per-project names for the 16 collision layer bits; index 0 is always "Default"
class CollisionLayers
{
public:
	static constexpr int MaxLayers = 16;
	static constexpr uint16_t DefaultLayer = 0x0001;
	static constexpr uint16_t Everything = 0xFFFF;

	using Names = std::array<std::string, MaxLayers>;

	static const char* FilePath;

	static Names DefaultNames();

	static bool Load(Names& names, const std::filesystem::path& filepath);
	static bool LoadFromData(Names& names, const std::vector<uint8_t>& data);
	static bool Save(const Names& names, const std::filesystem::path& filepath);

	static void SetNames(const Names& names);
	static void LoadNames(const std::filesystem::path& filepath);
	static void LoadNamesFromData(const std::vector<uint8_t>& data);
	static const Names& GetNames() { return s_Names; }

	// Empty names are unused slots
	static bool IsNamed(int index);

	// The layer's name, or "Layer <index>" for an unnamed slot
	static std::string GetDisplayName(int index);

	// -1 if no layer has this name
	static int GetIndex(const std::string& name);

	// Index of the lowest set bit, or -1 if none
	static int GetLayerIndex(uint16_t bits);

	static std::string GetLayerName(uint16_t layer);
	static std::vector<std::string> GetMaskNames(uint16_t mask);
	static bool MaskIncludes(uint16_t mask, const std::string& name);

private:
	static Names s_Names;
};

// Lua bindings shared by every collider component with `layer` and `mask` fields
#define REFLECT_LUA_COLLISION_FILTER() \
		type["Layer"] = sol::readonly_property([](Self& c) { return CollisionLayers::GetLayerName(c.layer); }); \
		RegisterLuaApiEntry({ "Layer", "Name of the collision layer this collider is on (read-only)", LuaApiEntry::Kind::Property, s_ReflectComponentName, "string", true }); \
		type["Mask"] = sol::readonly_property([](Self& c) { return sol::as_table(CollisionLayers::GetMaskNames(c.mask)); }); \
		RegisterLuaApiEntry({ "Mask", "Names of the collision layers this collider collides with (read-only)", LuaApiEntry::Kind::Property, s_ReflectComponentName, "table of string", true }); \
		REFLECT_LUA_FUNCTION_CUSTOM("CollidesWithLayer", "Whether this collider's mask includes the named collision layer", \
			[](Self& c, const std::string& name) { return CollisionLayers::MaskIncludes(c.mask, name); });
