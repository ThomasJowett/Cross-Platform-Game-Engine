#pragma once

#include <string>
#include <algorithm>
#include <vector>

// trim from start (in place)
static inline void ltrim(std::string& s)
{
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
		return !isspace(ch);
	}));
}

// trim from end (in place)
static inline void rtrim(std::string& s)
{
	s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
		return !isspace(ch);
		}).base(), s.end());
}

// splits a string into a vector
static std::vector<std::string> SplitString(const std::string &s, char delim)
{
	std::vector<std::string> elems;

	const char* cstr = s.c_str();
	size_t strLength = s.length();
	size_t start = 0;
	size_t end = 0;

	while (end <= strLength)
	{
		while (end <= strLength)
		{
			if (cstr[end] == delim)
				break;
			end++;
		}

		elems.push_back(s.substr(start, end - start));
		start = end + 1;
		end = start;
	}

	return elems;
}

static wchar_t* ConvertToWideChar(std::string& s)
{
	wchar_t* wideString = new wchar_t[s.length() + 1];
	std::copy(s.begin(), s.end(), wideString);
	wideString[s.length()] = 0;

	return wideString;
}