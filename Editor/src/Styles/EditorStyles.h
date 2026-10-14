#pragma once

#include <string>
#include <vector>

class EditorStyles
{
public:
	struct Theme
	{
		const char* name;
		// Applies the theme to the current ImGui style, returns true if it is a dark theme
		bool (*apply)();
	};

	static constexpr const char* DefaultTheme = "Attercop";

	static const std::vector<Theme>& GetThemes();

	// Applies the named theme, or the default theme if the name is unknown; returns true if it is a dark theme
	static bool Apply(const std::string& name);

	static bool Attercop();
	static bool Cobalt();
	static bool Phthalo();
	static bool Majorelle();
	static bool Cherry();
	static bool Xiketic();
	static bool Onyx();
	static bool Mono();
	static bool Emerald();
	static bool Ochre();
};
