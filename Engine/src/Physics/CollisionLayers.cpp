#include "CollisionLayers.h"

#include "TinyXml2/tinyxml2.h"
#include "Logging/Logger.h"

const char* CollisionLayers::FilePath = "Generated/CollisionLayers.layers";

CollisionLayers::Names CollisionLayers::s_Names = CollisionLayers::DefaultNames();

CollisionLayers::Names CollisionLayers::DefaultNames()
{
	Names names;
	names[0] = "Default";
	return names;
}

static bool LoadXML(CollisionLayers::Names& names, tinyxml2::XMLDocument& doc)
{
	tinyxml2::XMLElement* pRoot = doc.FirstChildElement("CollisionLayers");
	if (!pRoot)
	{
		ENGINE_ERROR("Could not read collision layers file, no CollisionLayers node");
		return false;
	}

	names = CollisionLayers::DefaultNames();

	for (tinyxml2::XMLElement* pLayer = pRoot->FirstChildElement("Layer"); pLayer; pLayer = pLayer->NextSiblingElement("Layer"))
	{
		int index = pLayer->IntAttribute("Index", -1);
		const char* name = pLayer->Attribute("Name");
		if (index <= 0 || index >= CollisionLayers::MaxLayers || !name)
			continue;
		names[index] = name;
	}

	return true;
}

bool CollisionLayers::Load(Names& names, const std::filesystem::path& filepath)
{
	if (!std::filesystem::exists(filepath))
		return false;

	tinyxml2::XMLDocument doc;
	if (doc.LoadFile(filepath.string().c_str()) != tinyxml2::XML_SUCCESS)
	{
		ENGINE_ERROR("Could not load collision layers file: {0}, {1} on line {2}", filepath.string(), doc.ErrorName(), doc.ErrorLineNum());
		return false;
	}

	return LoadXML(names, doc);
}

bool CollisionLayers::LoadFromData(Names& names, const std::vector<uint8_t>& data)
{
	tinyxml2::XMLDocument doc;
	if (doc.Parse((const char*)data.data(), data.size()) != tinyxml2::XML_SUCCESS)
	{
		ENGINE_ERROR("Could not parse collision layers data: {0} on line {1}", doc.ErrorName(), doc.ErrorLineNum());
		return false;
	}

	return LoadXML(names, doc);
}

bool CollisionLayers::Save(const Names& names, const std::filesystem::path& filepath)
{
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLElement* pRoot = doc.NewElement("CollisionLayers");
	doc.InsertFirstChild(pRoot);

	for (int i = 1; i < MaxLayers; ++i)
	{
		if (names[i].empty())
			continue;

		tinyxml2::XMLElement* pLayer = pRoot->InsertNewChildElement("Layer");
		pLayer->SetAttribute("Index", i);
		pLayer->SetAttribute("Name", names[i].c_str());
	}

	return doc.SaveFile(filepath.string().c_str()) == tinyxml2::XML_SUCCESS;
}

void CollisionLayers::SetNames(const Names& names)
{
	s_Names = names;
	s_Names[0] = "Default";
}

void CollisionLayers::LoadNames(const std::filesystem::path& filepath)
{
	Names names = DefaultNames();
	Load(names, filepath);
	SetNames(names);
}

void CollisionLayers::LoadNamesFromData(const std::vector<uint8_t>& data)
{
	Names names = DefaultNames();
	LoadFromData(names, data);
	SetNames(names);
}

bool CollisionLayers::IsNamed(int index)
{
	return index >= 0 && index < MaxLayers && !s_Names[index].empty();
}

std::string CollisionLayers::GetDisplayName(int index)
{
	if (IsNamed(index))
		return s_Names[index];
	return "Layer " + std::to_string(index);
}

int CollisionLayers::GetIndex(const std::string& name)
{
	for (int i = 0; i < MaxLayers; ++i)
	{
		if (!s_Names[i].empty() && s_Names[i] == name)
			return i;
	}
	return -1;
}

int CollisionLayers::GetLayerIndex(uint16_t bits)
{
	for (int i = 0; i < MaxLayers; ++i)
	{
		if (bits & (1 << i))
			return i;
	}
	return -1;
}

std::string CollisionLayers::GetLayerName(uint16_t layer)
{
	int index = GetLayerIndex(layer);
	return index >= 0 ? GetDisplayName(index) : std::string();
}

std::vector<std::string> CollisionLayers::GetMaskNames(uint16_t mask)
{
	std::vector<std::string> names;
	for (int i = 0; i < MaxLayers; ++i)
	{
		if ((mask & (1 << i)) && IsNamed(i))
			names.push_back(s_Names[i]);
	}
	return names;
}

bool CollisionLayers::MaskIncludes(uint16_t mask, const std::string& name)
{
	int index = GetIndex(name);
	return index >= 0 && (mask & (1 << index));
}
