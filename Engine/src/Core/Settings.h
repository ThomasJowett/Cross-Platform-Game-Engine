#pragma once

#include "stdafx.h"
#include <map>

class Settings
{
	static std::map<std::pair<std::string, std::string>, std::string> s_DefaultValues;

public:
	static void Init();
	static void SetValue(const char* section, const char* key, const char* value);
	static void SetBool(const char* section, const char* key, const bool value);
	static void SetDouble(const char* section, const char* key, const double value);
	static void SetInt(const char* section, const char* key, const int value);
	static void SetVector2f(const char* section, const char* key, const Vector2f& value);
	static void SetVector3f(const char* section, const char* key, const Vector3f& value);

	static const char* GetValue(const char* section, const char* key);
	static bool GetBool(const char* section, const char* key);
	static double GetDouble(const char* section, const char* key);
	static int GetInt(const char* section, const char* key);
	static Vector2f GetVector2f(const char* section, const char* key);
	static Vector3f GetVector3f(const char* section, const char* key);

	static void SetDefaultValue(const char* section, const char* key, const char* value);
	static void SetDefaultBool(const char* section, const char* key, const bool value);
	static void SetDefaultDouble(const char* section, const char* key, const double value);
	static void SetDefaultInt(const char* section, const char* key, const int value);
	static void SetDefaultVector2f(const char* section, const char* key, const Vector2f& value);
	static void SetDefaultVector3f(const char* section, const char* key, const Vector3f& value);

	static std::string GetDefaultValue(const char* section, const char* key);
	static bool GetDefaultBool(const char* section, const char* key);
	static double GetDefaultDouble(const char* section, const char* key);
	static int GetDefaultInt(const char* section, const char* key);
	static Vector2f GetDefaultVector2f(const char* section, const char* key);
	static Vector3f GetDefaultVector3f(const char* section, const char* key);

	static void SaveSettings();
};