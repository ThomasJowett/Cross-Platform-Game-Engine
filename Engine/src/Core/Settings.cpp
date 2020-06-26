#include "stdafx.h"
#include "Settings.h"

#include "simpleini.h"

#include "Utilities/StringUtils.h"

static Settings* s_Instance = nullptr;

static CSimpleIniA* s_Ini = nullptr;
const char* Settings::s_Filename = "Settings.ini";

std::map<std::pair<std::string, std::string>, std::string> Settings::s_DefaultValues = {};

void Settings::Init()
{
	s_Ini = new CSimpleIniA();

	s_Ini->SetUnicode();

	if (!std::filesystem::exists(s_Filename))
	{
		std::ofstream outputStream;

		outputStream.open(s_Filename);

		if (outputStream.good())
		{
			ENGINE_INFO("Created Settings file: {0}", s_Filename);
		}
		else
		{
			ENGINE_ERROR("Could not create settings file: {0}", s_Filename);
		}
	}

	s_Ini->LoadFile(s_Filename);
}

void Settings::SetValue(const char* section, const char* key, const char* value)
{
	SI_Error rc = s_Ini->SetValue(section, key, value);

	if (rc < 0)
	{
		ENGINE_ERROR("Failed to set setting: {0}", rc);
	}
}

void Settings::SetBool(const char* section, const char* key, const bool value)
{
	SI_Error rc = s_Ini->SetBoolValue(section, key, value);

	if (rc < 0)
	{
		ENGINE_ERROR("Failed to set bool setting: {0}", rc);
	}
}

void Settings::SetDouble(const char* section, const char* key, const double value)
{
	SI_Error rc = s_Ini->SetDoubleValue(section, key, value);

	if (rc < 0)
	{
		ENGINE_ERROR("Failed to set bool setting: {0}", rc);
	}
}

void Settings::SetInt(const char* section, const char* key, const int value)
{
	SI_Error rc = s_Ini->SetLongValue(section, key, value);

	if (rc < 0)
	{
		ENGINE_ERROR("Failed to set integer setting: {0}", rc);
	}
}

void Settings::SetVector2f(const char* section, const char* key, const Vector2f& value)
{
	std::string valueString = std::to_string(value.x) + "," + std::to_string(value.y);

	SI_Error rc = s_Ini->SetValue(section, key, valueString.c_str());

	if (rc < 0)
	{
		ENGINE_ERROR("Failed to set Vector2f setting: {0}", rc);
	}
}

void Settings::SetVector3f(const char* section, const char* key, const Vector3f& value)
{
	std::string valueString = std::to_string(value.x) + "," + std::to_string(value.y) + "," + std::to_string(value.z);

	SI_Error rc = s_Ini->SetValue(section, key, valueString.c_str());

	if (rc < 0)
	{
		ENGINE_ERROR("Failed to set Vector2f setting: {0}", rc);
	}
}

const char* Settings::GetValue(const char* section, const char* key)
{
	if (s_DefaultValues.find({ section, key }) == s_DefaultValues.end())
	{
		ENGINE_WARN("No default setting set for: [{0}] {1}", section, key);
		return s_Ini->GetValue(section, key);
	}

	return s_Ini->GetValue(section, key, s_DefaultValues.at({ section, key }).c_str());
}

bool Settings::GetBool(const char* section, const char* key)
{
	if (s_DefaultValues.find({ section, key }) == s_DefaultValues.end())
	{
		ENGINE_WARN("No default setting for [{0}] {1}", section, key);
		return s_Ini->GetBoolValue(section, key);
	}

	return s_Ini->GetBoolValue(section, key, s_DefaultValues.at({ section, key }) == "true");
}

double Settings::GetDouble(const char* section, const char* key)
{
	if (s_DefaultValues.find({ section, key }) == s_DefaultValues.end())
	{
		ENGINE_WARN("No default setting for [{0}] {1}", section, key);
		return s_Ini->GetDoubleValue(section, key);
	}

	return s_Ini->GetDoubleValue(section, key, atof(s_DefaultValues.at({ section, key }).c_str()));
}

int Settings::GetInt(const char* section, const char* key)
{
	if (s_DefaultValues.find({ section, key }) == s_DefaultValues.end())
	{
		ENGINE_WARN("No default setting for [{0}] {1}", section, key);
		return s_Ini->GetLongValue(section, key);
	}
	return s_Ini->GetLongValue(section, key, atoi(s_DefaultValues.at({ section, key }).c_str()));
}

Vector2f Settings::GetVector2f(const char* section, const char* key)
{
	if (s_DefaultValues.find({ section, key }) == s_DefaultValues.end())
	{
		ENGINE_WARN("No default setting for [{0}] {1}", section, key);
	}

	const char* vector = s_Ini->GetValue(section, key, s_DefaultValues.at({ section, key }).c_str());

	std::vector<std::string> splitVector = SplitString(vector, ',');
	if (splitVector.size() == 2)
		return Vector2f((float)atof(splitVector[0].c_str()), (float)atof(splitVector[1].c_str()));

	ENGINE_ERROR("Could not load vector2f setting: [{0}] {1}", section, key);
	return Vector2f();
}

Vector3f Settings::GetVector3f(const char* section, const char* key)
{
	if (s_DefaultValues.find({ section, key }) == s_DefaultValues.end())
	{
		ENGINE_WARN("No default setting for [{0}] {1}", section, key);
	}

	const char* vector = s_Ini->GetValue(section, key, s_DefaultValues.at({ section, key }).c_str());

	std::vector<std::string> splitVector = SplitString(vector, ',');
	if (splitVector.size() == 3)
		return Vector3f((float)atof(splitVector[0].c_str()), (float)atof(splitVector[1].c_str()), (float)atof(splitVector[2].c_str()));

	ENGINE_ERROR("Could not load vector3f setting: [{0}] {1}", section, key);
	return Vector3f();
}

void Settings::SetDefaultValue(const char* section, const char* key, const char* value)
{
	s_DefaultValues[{section, key}] = value;
}

void Settings::SetDefaultBool(const char* section, const char* key, const bool value)
{
	s_DefaultValues[{section, key}] = value ? "true" : "false";
}

void Settings::SetDefaultDouble(const char* section, const char* key, const double value)
{
	s_DefaultValues[{section, key}] = std::to_string(value);
}

void Settings::SetDefaultInt(const char* section, const char* key, const int value)
{
	s_DefaultValues[{section, key}] = std::to_string(value);
}

void Settings::SetDefaultVector2f(const char* section, const char* key, const Vector2f& value)
{
	s_DefaultValues[{section, key}] = std::to_string(value.x) + ',' + std::to_string(value.y);
}

void Settings::SetDefaultVector3f(const char* section, const char* key, const Vector3f& value)
{
	s_DefaultValues[{section, key}] = std::to_string(value.x) + ',' + std::to_string(value.y) + ',' + std::to_string(value.z);
}

void Settings::SaveSettings()
{
	SI_Error rc = s_Ini->SaveFile(s_Filename);

	if (rc < 0)
	{
		ENGINE_ERROR("Failed to save settings to file: {0}", rc);
	}
}
