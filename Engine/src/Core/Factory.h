#pragma once
#include <unordered_map>
#include <string>

template<typename T, typename... Args>
struct Factory
{
	typedef std::unordered_map<std::string, std::function<T* (Args...)>>map_Type;

	static T* CreateInstance(const std::string& name, Args... args)
	{
		typename map_Type::iterator it = GetMap()->find(name);

		return (it == GetMap()->end() ? nullptr : it->second(args...));
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