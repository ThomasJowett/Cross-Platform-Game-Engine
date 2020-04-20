#pragma once

#include "stdafx.h"

#include "simpleini.h"

class Settings
{
	static const char* s_Filename;

public:
	void SetValue(const char* section, const char* key, const char* value);

	const char* GetValue(const char* section, const char* key);
	bool GetBool(const char* section, const char* key);
	double GetDouble(const char* section, const char* key);
	int GetInt(const char* section, const char* key);
	Vector2f GetVector2f(const char* section, const char* key);
	Vector3f GetVector3f(const char* section, const char* key);

private:

};