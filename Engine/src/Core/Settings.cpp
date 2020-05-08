#include "stdafx.h"
#include "Settings.h"

#include "simpleini.h"

#include "Utilities/StringUtils.h"

static Settings* s_Instance = nullptr;

static CSimpleIniA* s_Ini = nullptr;
const char* Settings::s_Filename = "Settings.ini";

Settings::Settings()
{
	s_Instance = this;

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

Settings::~Settings()
{
	s_Ini->SaveFile(s_Filename);
}

Settings* Settings::Get()
{
	if (s_Instance == nullptr)
		return new Settings();

	return s_Instance;
}

void Settings::SetValueImpl(const char* section, const char* key, const char* value)
{
	SI_Error rc = s_Ini->SetValue(section, key, value);

	if (rc < 0)
	{
		ENGINE_ERROR("Failed to set setting: {0}", rc);
	}
}

void Settings::SetBoolImpl(const char* section, const char* key, const bool value)
{
	SI_Error rc = s_Ini->SetBoolValue(section, key, value);

	if (rc < 0)
	{
		ENGINE_ERROR("Failed to set bool setting: {0}", rc);
	}
}

void Settings::SetDoubleImpl(const char* section, const char* key, const double value)
{
	SI_Error rc = s_Ini->SetDoubleValue(section, key, value);

	if (rc < 0)
	{
		ENGINE_ERROR("Failed to set bool setting: {0}", rc);
	}
}

void Settings::SetIntImpl(const char* section, const char* key, const int value)
{
	SI_Error rc = s_Ini->SetLongValue(section, key, value);

	if (rc < 0)
	{
		ENGINE_ERROR("Failed to set integer setting: {0}", rc);
	}
}

void Settings::SetVector2fImpl(const char* section, const char* key, const Vector2f& value)
{
	std::string valueString = std::to_string(value.x) + "," + std::to_string(value.y);

	SI_Error rc = s_Ini->SetValue(section, key, valueString.c_str());

	if (rc < 0)
	{
		ENGINE_ERROR("Failed to set Vector2f setting: {0}", rc);
	}
}

void Settings::SetVector3fImpl(const char* section, const char* key, const Vector3f& value)
{
	std::string valueString = std::to_string(value.x) + "," + std::to_string(value.y) + "," + std::to_string(value.z);

	SI_Error rc = s_Ini->SetValue(section, key, valueString.c_str());

	if (rc < 0)
	{
		ENGINE_ERROR("Failed to set Vector2f setting: {0}", rc);
	}
}

const char* Settings::GetValueImpl(const char* section, const char* key)
{
	if (m_DefaultValues.find({ section, key }) == m_DefaultValues.end())
	{
		ENGINE_WARN("No default setting set for: [{0}] {1}", section, key);
		return s_Ini->GetValue(section, key);
	}

	return s_Ini->GetValue(section, key, m_DefaultValues.at({ section, key }).c_str());
}

bool Settings::GetBoolImpl(const char* section, const char* key)
{
	if (m_DefaultValues.find({ section, key }) == m_DefaultValues.end())
	{
		ENGINE_WARN("No default setting for [{0}] {1}", section, key);
		return s_Ini->GetBoolValue(section, key);
	}

	return s_Ini->GetBoolValue(section, key, m_DefaultValues.at({ section, key }) == "true");
}

double Settings::GetDoubleImpl(const char* section, const char* key)
{
	if (m_DefaultValues.find({ section, key }) == m_DefaultValues.end())
	{
		ENGINE_WARN("No default setting for [{0}] {1}", section, key);
		return s_Ini->GetDoubleValue(section, key);
	}

	return s_Ini->GetDoubleValue(section, key, atof(m_DefaultValues.at({ section, key }).c_str()));
}

int Settings::GetIntImpl(const char* section, const char* key)
{
	if (m_DefaultValues.find({ section, key }) == m_DefaultValues.end())
	{
		ENGINE_WARN("No default setting for [{0}] {1}", section, key);
		return s_Ini->GetLongValue(section, key);
	}
	return s_Ini->GetLongValue(section, key, atoi(m_DefaultValues.at({ section, key }).c_str()));
}

Vector2f Settings::GetVector2fImpl(const char* section, const char* key)
{
	if (m_DefaultValues.find({ section, key }) == m_DefaultValues.end())
	{
		ENGINE_WARN("No default setting for [{0}] {1}", section, key);
	}

	const char* vector = s_Ini->GetValue(section, key, m_DefaultValues.at({ section, key }).c_str());

	std::vector<std::string> splitVector = SplitString(vector, ',');
	if (splitVector.size() == 2)
		return Vector2f((float)atof(splitVector[0].c_str()), (float)atof(splitVector[1].c_str()));

	ENGINE_ERROR("Could not load vector2f setting: [{0}] {1}", section, key);
	return Vector2f();
}

Vector3f Settings::GetVector3fImpl(const char* section, const char* key)
{
	if (m_DefaultValues.find({ section, key }) == m_DefaultValues.end())
	{
		ENGINE_WARN("No default setting for [{0}] {1}", section, key);
	}

	const char* vector = s_Ini->GetValue(section, key, m_DefaultValues.at({ section, key }).c_str());

	std::vector<std::string> splitVector = SplitString(vector, ',');
	if (splitVector.size() == 3)
		return Vector3f((float)atof(splitVector[0].c_str()), (float)atof(splitVector[1].c_str()), (float)atof(splitVector[2].c_str()));

	ENGINE_ERROR("Could not load vector3f setting: [{0}] {1}", section, key);
	return Vector3f();
}

void Settings::SetDefaultValueImpl(const char* section, const char* key, const char* value)
{
	m_DefaultValues[{section, key}] = value;
}

void Settings::SetDefaultBoolImpl(const char* section, const char* key, const bool value)
{
	m_DefaultValues[{section, key}] = value ? "true" : "false";
}

void Settings::SetDefaultDoubleImpl(const char* section, const char* key, const double value)
{
	m_DefaultValues[{section, key}] = std::to_string(value);
}

void Settings::SetDefaultIntImpl(const char* section, const char* key, const int value)
{
	m_DefaultValues[{section, key}] = std::to_string(value);
}

void Settings::SetDefaultVector2fImpl(const char* section, const char* key, const Vector2f& value)
{
	m_DefaultValues[{section, key}] = std::to_string(value.x) + ',' + std::to_string(value.y);
}

void Settings::SetDefaultVector3fImpl(const char* section, const char* key, const Vector3f& value)
{
	m_DefaultValues[{section, key}] = std::to_string(value.x) + ',' + std::to_string(value.y) + ',' + std::to_string(value.z);
}

void Settings::SaveFile()
{
	SI_Error rc = s_Ini->SaveFile(s_Filename);

	if (rc < 0)
	{
		ENGINE_ERROR("Failed to save settings to file: {0}", rc);
	}
}
