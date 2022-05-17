#include "TiledImporter.h"
#include "TinyXml2/tinyxml2.h"
#include "Engine.h"

static TilemapComponent::Orientation s_Orientation;
static std::map<uint32_t, Ref<Tileset>> s_Tilesets;

bool ParseCsv(const std::string& data, TilemapComponent& tilemapComp)
{
	tilemapComp.tiles = new uint32_t * [tilemapComp.tilesHigh];

	for (uint32_t i = 0; i < tilemapComp.tilesHigh; i++)
	{
		tilemapComp.tiles[i] = new uint32_t[tilemapComp.tilesWide];
	}

	std::vector<std::string> seperatedData = SplitString(data, ',');

	ASSERT((uint32_t)seperatedData.size() == tilemapComp.tilesWide * tilemapComp.tilesHigh, "Data not the correct length");

	for (size_t i = 0; i < tilemapComp.tilesHigh; i++)
	{
		for (size_t j = 0; j < tilemapComp.tilesWide; j++)
		{
			uint32_t index = (uint32_t)atoi(seperatedData[(i * (static_cast<size_t>(tilemapComp.tilesWide))) + j].c_str());
			tilemapComp.tiles[i][j] = index;
		}
	}

	return false;
}

Entity LoadObjectGroup(tinyxml2::XMLElement* pObjectGroup)
{
	//TODO: load objects

	std::string groupName = pObjectGroup->Attribute("name");

	const char* verticalOffsetChar = pObjectGroup->Attribute("offsetx");
	const char* horizontalOffsetChar = pObjectGroup->Attribute("offsety");

	Vector3f offset(
		verticalOffsetChar != nullptr ? atoi(verticalOffsetChar) : 0.0f,
		horizontalOffsetChar != nullptr ? atoi(horizontalOffsetChar) : 0.0f,
		0.0f
	);
	Entity groupEntity = SceneManager::CurrentScene()->CreateEntity(groupName);
	groupEntity.GetTransform().position += offset;

	tinyxml2::XMLElement* pObject = pObjectGroup->FirstChildElement("object");

	while (pObject)
	{
		std::string objectName = pObject->Attribute("name");
		Entity entity = SceneManager::CurrentScene()->CreateEntity(objectName);
		groupEntity.AddChild(entity);

		pObject = pObject->NextSiblingElement("object");
	}

	return groupEntity;
}

Entity LoadLayer(tinyxml2::XMLElement* pLayer)
{
	std::string name = pLayer->Attribute("name");

	const char* verticalOffsetChar = pLayer->Attribute("offsetx");
	const char* horizontalOffsetChar = pLayer->Attribute("offsety");

	Vector3f offset(
		verticalOffsetChar != nullptr ? atoi(verticalOffsetChar) : 0.0f,
		horizontalOffsetChar != nullptr ? atoi(horizontalOffsetChar) : 0.0f,
		0.0f
	);

	uint32_t width = atoi(pLayer->Attribute("width"));
	uint32_t height = atoi(pLayer->Attribute("height"));

	Entity entity = SceneManager::CurrentScene()->CreateEntity(name);
	TilemapComponent& tilemapComp = entity.AddComponent<TilemapComponent>(s_Orientation, width, height);
	entity.GetTransform().position += offset;


	tinyxml2::XMLElement* pData = pLayer->FirstChildElement("data");

	const char* encoding = pData->Attribute("encoding");

	if (!strcmp(encoding, "csv"))
	{
		if (!ParseCsv(pData->GetText(), entity.GetComponent<TilemapComponent>()))
		{
			ENGINE_ERROR("Could not parse tilemap layer {0}", name);
		}
	}
	else if (!strcmp(encoding, "base64"))
	{
		ENGINE_ERROR("Could not load tilemap. {0} encoding not yet supported", encoding);
	}
	else
	{
		ENGINE_ERROR("Could not load tilemap. {0} encoding not recognised", encoding);
	}

	for (size_t i = 0; i < height; i++)
	{
		for (size_t j = 0; j < width; j++)
		{
			if (tilemapComp.tiles[i][j] == 0)
				continue;

			Ref<Tileset> tileset;
			// find tileset to use
			uint32_t gid = 1;
			for (auto& [id, tileset] : s_Tilesets)
			{
				if (tilemapComp.tiles[i][j] > (id - 1) && (id - 1) >= gid)
				{
					gid = id;
				}
				if (id - 1 > tilemapComp.tiles[i][j])
				{
					break;
				}
			}
			tileset = s_Tilesets.at(gid);
		}
	}

	return entity;
}

Entity LoadGroup(tinyxml2::XMLElement* pGroup)
{
	std::string groupName = pGroup->Attribute("name");
	Entity groupEntity = SceneManager::CurrentScene()->CreateEntity(groupName);

	const char* verticalOffsetChar = pGroup->Attribute("offsetx");
	const char* horizontalOffsetChar = pGroup->Attribute("offsety");

	Vector3f offset(
		verticalOffsetChar != nullptr ? atoi(verticalOffsetChar) : 0.0f,
		horizontalOffsetChar != nullptr ? atoi(horizontalOffsetChar) : 0.0f,
		0.0f
	);
	groupEntity.GetTransform().position += offset;

	tinyxml2::XMLElement* pLayer = pGroup->FirstChildElement("layer");

	while (pLayer)
	{
		groupEntity.AddChild(LoadLayer(pLayer));

		pLayer = pLayer->NextSiblingElement("layer");
	}

	tinyxml2::XMLElement* pObjectGroup = pGroup->FirstChildElement("objectgroup");

	while (pObjectGroup)
	{
		groupEntity.AddChild(LoadObjectGroup(pObjectGroup));

		pObjectGroup = pObjectGroup->NextSiblingElement("layer");
	}

	tinyxml2::XMLElement* pChildGroup = pGroup->FirstChildElement("group");

	while (pChildGroup)
	{
		groupEntity.AddChild(LoadGroup(pChildGroup));

		pChildGroup = pChildGroup->NextSiblingElement("group");
	}

	return groupEntity;
}

void TiledImporter::ImportAssets(const std::filesystem::path& filepath, const std::filesystem::path& destination)
{
	tinyxml2::XMLDocument doc;

	if (doc.LoadFile(filepath.string().c_str()) == tinyxml2::XML_SUCCESS)
	{
		tinyxml2::XMLElement* pRoot = doc.FirstChildElement("map");

		uint32_t width, height;

		const char* orientationchar = pRoot->Attribute("orientation");

		if (orientationchar != nullptr)
		{
			std::string orientationStr(orientationchar);

			if (orientationStr == "orthogonal")
				s_Orientation = TilemapComponent::Orientation::orthogonal;
			else if (orientationStr == "isometric")
				s_Orientation = TilemapComponent::Orientation::isometric;
			else if (orientationStr == "staggered")
				s_Orientation = TilemapComponent::Orientation::staggered;
			else if (orientationStr == "hexagonal")
				s_Orientation = TilemapComponent::Orientation::hexagonal;
		}

		pRoot->QueryUnsignedAttribute("width", &width);
		pRoot->QueryUnsignedAttribute("height", &height);

		tinyxml2::XMLElement* pTileSet = pRoot->FirstChildElement("tileset");

		std::filesystem::path fileDirectory = filepath;
		fileDirectory.remove_filename();

		while (pTileSet)
		{
			const char* tsxPath = pTileSet->Attribute("source");

			std::filesystem::path tilesetPath(fileDirectory / tsxPath);

			Ref<Tileset> tileset = CreateRef<Tileset>();
			if (tileset->Load(tilesetPath))
				s_Tilesets.insert(std::make_pair(atoi(pTileSet->Attribute("firstgid")), tileset));

			pTileSet = pTileSet->NextSiblingElement("tileset");
		}

		// Layers ------------------------------------------------------------------------------------------------------
		tinyxml2::XMLElement* pLayer = pRoot->FirstChildElement("layer");

		while (pLayer)
		{
			LoadLayer(pLayer);

			pLayer = pLayer->NextSiblingElement("layer");
		}

		// object groups ------------------------------------------------------------------------------------------------------
		tinyxml2::XMLElement* pObjectGroup = pRoot->FirstChildElement("objectgroup");

		while (pObjectGroup)
		{
			LoadObjectGroup(pObjectGroup);

			pObjectGroup = pObjectGroup->NextSiblingElement("objectgroup");
		}

		// Groups --------------------------------------------------------------------------------------------------------
		tinyxml2::XMLElement* pGroup = pRoot->FirstChildElement("group");

		while (pGroup)
		{
			LoadGroup(pGroup);

			pGroup = pGroup->NextSiblingElement("group");
		}
	}
	else
	{
		CLIENT_ERROR("Could not load tilemap {0}. {1} on line {2}", filepath, doc.ErrorName(), doc.ErrorLineNum());
	}
}
