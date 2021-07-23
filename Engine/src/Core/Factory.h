#pragma once
#include <unordered_map>
#include <string>

template<typename T>
struct Factory
{
	typedef std::unordered_map<std::string, T*(*)()>map_Type;

	template<typename... Args>
	static T* CreateInstance(const std::string& name)
	{
		typename map_Type::iterator it = GetMap()->find(name);

		return (it == GetMap()->end() ? nullptr : it->second());
	}

	static map_Type* GetMap()
	{
		if (!s_LookUpTable)
		{
			s_LookUpTable = new map_Type;
		}
		return s_LookUpTable;
	}
private:
	static map_Type* s_LookUpTable;
};