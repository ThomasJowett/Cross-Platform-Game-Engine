#pragma once

#include "stdafx.h"

#include "simpleini.h"

class Settings
{
	static const char* s_Filename;

public:
	static void SetValue(const char* section, const char* key, const char* value) { Get()->SetValueImpl(section, key, value); }
	static void SetBool(const char* section, const char* key, const bool value) { Get()->SetBoolImpl(section, key, value); }
	static void SetDouble(const char* section, const char* key, const double value) { Get()->SetDoubleImpl(section, key, value); }
	static void SetInt(const char* section, const char* key, const int value) { Get()->SetIntImpl(section, key, value); }
	static void SetVector2f(const char* section, const char* key, const Vector2f& value) { Get()->SetVector2fImpl(section, key, value); }
	static void SetVector3f(const char* section, const char* key, const Vector3f& value) { Get()->SetVector3fImpl(section, key, value); }

	static const char* GetValue(const char* section, const char* key) { return Get()->GetValueImpl(section, key); }
	static bool GetBool(const char* section, const char* key) { return Get()->GetBoolImpl(section, key); }
	static double GetDouble(const char* section, const char* key) { return Get()->GetDoubleImpl(section, key); }
	static int GetInt(const char* section, const char* key) { return Get()->GetIntImpl(section, key); }
	static Vector2f GetVector2f(const char* section, const char* key) { return Get()->GetVector2fImpl(section, key); }
	static Vector3f GetVector3f(const char* section, const char* key) { return Get()->GetVector3fImpl(section, key); }

	static void SetDefaultValue(const char* section, const char* key, const char* value) { Get()->SetDefaultValueImpl(section, key, value); }
	static void SetDefaultBool(const char* section, const char* key, const bool value) { Get()->SetDefaultBoolImpl(section, key, value); }
	static void SetDefaultDouble(const char* section, const char* key, const double value) { Get()->SetDefaultDoubleImpl(section, key, value); }
	static void SetDefaultInt(const char* section, const char* key, const int value) { Get()->SetDefaultIntImpl(section, key, value); }
	static void SetDefaultVector2f(const char* section, const char* key, const Vector2f& value) { Get()->SetDefaultVector2fImpl(section, key, value); }
	static void SetDefaultVector3f(const char* section, const char* key, const Vector3f& value) { Get()->SetDefaultVector3fImpl(section, key, value); }

	static void SaveSettings() { Get()->SaveFile(); }
private:
	Settings();
	~Settings();

	static Settings* Get();

	void SetValueImpl(const char* section, const char* key, const char* value);
	void SetBoolImpl(const char* section, const char* key, const bool value);
	void SetDoubleImpl(const char* section, const char* key, const double value);
	void SetIntImpl(const char* section, const char* key, const int value);
	void SetVector2fImpl(const char* section, const char* key, const Vector2f& value);
	void SetVector3fImpl(const char* section, const char* key, const Vector3f& value);

	const char* GetValueImpl(const char* section, const char* key);
	bool GetBoolImpl(const char* section, const char* key);
	double GetDoubleImpl(const char* section, const char* key);
	int GetIntImpl(const char* section, const char* key);
	Vector2f GetVector2fImpl(const char* section, const char* key);
	Vector3f GetVector3fImpl(const char* section, const char* key);

	void SetDefaultValueImpl(const char* section, const char* key, const char* value);
	void SetDefaultBoolImpl(const char* section, const char* key, const bool value);
	void SetDefaultDoubleImpl(const char* section, const char* key, const double value);
	void SetDefaultIntImpl(const char* section, const char* key, const int value);
	void SetDefaultVector2fImpl(const char* section, const char* key, const Vector2f& value);
	void SetDefaultVector3fImpl(const char* section, const char* key, const Vector3f& value);

	void SaveFile();
private:
	CSimpleIniA* m_Ini;

	std::map<std::pair<const char*, const char*>, const char*> m_DefaultValues;
};