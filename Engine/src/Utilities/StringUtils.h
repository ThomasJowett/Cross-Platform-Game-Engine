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
static std::vector<std::string> SplitString(const std::string& s, char delim)
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

static wchar_t* ConvertToWideChar(const std::string& s)
{
	wchar_t* wideString = new wchar_t[s.length() + 1];
	std::copy(s.begin(), s.end(), wideString);
	wideString[s.length()] = 0;

	return wideString;
}

template <std::size_t...Idxs>
constexpr auto substring_as_array(std::string_view str, std::index_sequence<Idxs...>)
{
	return std::array{ str[Idxs]..., '\n' };
}

template <typename T>
constexpr auto type_name_array()
{
#if defined(__clang__)
	constexpr auto prefix = std::string_view{ "[T = " };
	constexpr auto suffix = std::string_view{ "]" };
	constexpr auto function = std::string_view{ __PRETTY_FUNCTION__ };
#elif defined(__GNUC__)
	constexpr auto prefix = std::string_view{ "with T = " };
	constexpr auto suffix = std::string_view{ "]" };
	constexpr auto function = std::string_view{ __PRETTY_FUNCTION__ };
#elif defined(_MSC_VER)
	constexpr std::string_view prefix("type_name_array<");
	constexpr std::string_view suffix(">(void)");
	constexpr std::string_view function( __FUNCSIG__ );
#else
# error Unsupported compiler
#endif

	constexpr auto start = function.find(prefix) + prefix.size();
	constexpr auto end = function.rfind(suffix);

	static_assert(start < end);

	constexpr auto fullname = function.substr(start, (end - start));
	constexpr auto name = fullname.substr(fullname.find(" ") + 1);
	return substring_as_array(name, std::make_index_sequence<name.size()>{});
}

template <typename T>
struct type_name_holder {
	static inline constexpr auto value = type_name_array<T>();
};

template <typename T>
constexpr auto type_name() -> std::string_view
{
	constexpr auto& value = type_name_holder<T>::value;
	return std::string_view{ value.data(), value.size() };
}