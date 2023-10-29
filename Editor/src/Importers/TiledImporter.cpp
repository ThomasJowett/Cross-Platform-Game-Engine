#include "TiledImporter.h"
#include "TinyXml2/tinyxml2.h"
#include "Engine.h"
#include "Utilities/SerializationUtils.h"

static TilemapComponent::Orientation s_Orientation;
static std::map<uint32_t, Ref<Tileset>> s_Tilesets;
static std::filesystem::path s_Filepath;

static uint32_t s_TileWidth, s_TileHeight;

bool ParseCsv(const std::string& data, TilemapComponent& tilemapComp)
{
	tilemapComp.tiles.reserve(tilemapComp.tilesHigh);

	for (uint32_t i = 0; i < tilemapComp.tilesHigh; i++)
	{
		tilemapComp.tiles[i].reserve(tilemapComp.tilesWide);
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

	return true;
}

Entity LoadImageLayer(tinyxml2::XMLElement* pImageLayer)
{
	const char* layerName = pImageLayer->Attribute("name");

	const char* horizontalOffsetChar = pImageLayer->Attribute("offsetx");
	const char* verticalOffsetChar = pImageLayer->Attribute("offsety");
	const char* tintColour = pImageLayer->Attribute("tintcolor");

	Vector3f offset(
		horizontalOffsetChar != nullptr ? atoi(horizontalOffsetChar) / s_TileWidth : 0.0f,
		verticalOffsetChar != nullptr ? atoi(verticalOffsetChar) / s_TileHeight : 0.0f,
		0.0f
	);

	tinyxml2::XMLElement* pImage = pImageLayer->FirstChildElement("image");

	Entity imageLayerEntity = SceneManager::CurrentScene()->CreateEntity(layerName ? layerName : "Image");

	TransformComponent& transformComp = imageLayerEntity.GetOrAddComponent<TransformComponent>();
	transformComp.position = offset;
	transformComp.scale = Vector3f((float)s_TileWidth, (float)s_TileHeight, 1.0f);

	SpriteComponent& spriteComp = imageLayerEntity.AddComponent<SpriteComponent>();

	const char* textureSource = pImage->Attribute("source");
	if (textureSource)
	{
		std::filesystem::path texturePath = s_Filepath;
		texturePath.remove_filename();
		texturePath = texturePath / textureSource;
		spriteComp.texture = AssetManager::GetTexture(texturePath);
	}

	if (tintColour)
	{
		spriteComp.tint.SetColour(tintColour);
	}

	return imageLayerEntity;
}

Entity LoadObjectGroup(tinyxml2::XMLElement* pObjectGroup)
{
	//TODO: load objects

	std::string groupName = pObjectGroup->Attribute("name");

	const char* horizontalOffsetChar = pObjectGroup->Attribute("offsetx");
	const char* verticalOffsetChar = pObjectGroup->Attribute("offsety");

	Vector3f offset(
		horizontalOffsetChar != nullptr ? atoi(horizontalOffsetChar) / s_TileWidth : 0.0f,
		verticalOffsetChar != nullptr ? atoi(verticalOffsetChar) / s_TileHeight : 0.0f,
		0.0f
	);
	Entity groupEntity = SceneManager::CurrentScene()->CreateEntity(groupName);
	TransformComponent& transformComp = groupEntity.GetOrAddComponent<TransformComponent>();
	transformComp.position = offset;

	tinyxml2::XMLElement* pObject = pObjectGroup->FirstChildElement("object");

	while (pObject)
	{
		const char* objectName = pObject->Attribute("name");
		Entity entity = SceneManager::CurrentScene()->CreateEntity(objectName ? objectName : "Tiled Object");
		TransformComponent& transformComp = entity.GetOrAddComponent<TransformComponent>();
		groupEntity.AddChild(entity);

		pObject = pObject->NextSiblingElement("object");
	}

	return groupEntity;
}

Entity LoadLayer(tinyxml2::XMLElement* pLayer)
{
	std::string name = pLayer->Attribute("name");

	const char* horizontalOffsetChar = pLayer->Attribute("offsetx");
	const char* verticalOffsetChar = pLayer->Attribute("offsety");

	Vector3f offset(
		horizontalOffsetChar != nullptr ? atoi(horizontalOffsetChar) / s_TileWidth : 0.0f,
		verticalOffsetChar != nullptr ? atoi(verticalOffsetChar) / s_TileHeight : 0.0f,
		0.0f
	);

	uint32_t width = atoi(pLayer->Attribute("width"));
	uint32_t height = atoi(pLayer->Attribute("height"));

	Entity entity = SceneManager::CurrentScene()->CreateEntity(name);
	TilemapComponent& tilemapComp = entity.AddComponent<TilemapComponent>(s_Orientation, width, height);
	TransformComponent& transformComp = entity.GetOrAddComponent<TransformComponent>();
	transformComp.position = offset;

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

			// find tileset to use
			uint32_t gid = 1;
			for (auto&& [id, tileset] : s_Tilesets)
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
			tilemapComp.tileset = s_Tilesets.at(gid);
		}
	}

	return entity;
}

Entity LoadGroup(tinyxml2::XMLElement* pGroup)
{
	std::string groupName = pGroup->Attribute("name");
	Entity groupEntity = SceneManager::CurrentScene()->CreateEntity(groupName);
	TransformComponent& transformComp = groupEntity.GetOrAddComponent<TransformComponent>();

	const char* horizontalOffsetChar = pGroup->Attribute("offsetx");
	const char* verticalOffsetChar = pGroup->Attribute("offsety");

	Vector3f offset(
		horizontalOffsetChar != nullptr ? atoi(horizontalOffsetChar) / s_TileWidth : 0.0f,
		verticalOffsetChar != nullptr ? atoi(verticalOffsetChar) / s_TileHeight : 0.0f,
		0.0f
	);
	transformComp.position = offset;

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

		pObjectGroup = pObjectGroup->NextSiblingElement("objectgroup");
	}

	tinyxml2::XMLElement* pImageGroup = pGroup->FirstChildElement("imagelayer");

	while (pImageGroup)
	{
		groupEntity.AddChild(LoadImageLayer(pImageGroup));

		pImageGroup = pImageGroup->NextSiblingElement("imagelayer");
	}

	tinyxml2::XMLElement* pChildGroup = pGroup->FirstChildElement("group");

	while (pChildGroup)
	{
		groupEntity.AddChild(LoadGroup(pChildGroup));

		pChildGroup = pChildGroup->NextSiblingElement("group");
	}

	return groupEntity;
}

void LoadTileset(const std::filesystem::path& filepath, const std::filesystem::path& destination)
{
	tinyxml2::XMLDocument doc;

	if (doc.LoadFile(filepath.string().c_str()) == tinyxml2::XML_SUCCESS)
	{
		Ref<Tileset> tileset = CreateRef<Tileset>();
		tinyxml2::XMLElement* pRoot;

		pRoot = doc.FirstChildElement("tileset");

		uint32_t tileWidth, tileHeight;

		pRoot->QueryUnsignedAttribute("tilewidth", &tileWidth);
		pRoot->QueryUnsignedAttribute("tileheight", &tileHeight);

		tinyxml2::XMLElement* pImage = pRoot->FirstChildElement("image");

		const char* textureSource = pImage->Attribute("source");

		if (textureSource)
		{
			std::filesystem::path texturePath = filepath;
			texturePath.remove_filename();
			texturePath = texturePath / textureSource;
			tileset->SetSubTexture(CreateRef<SubTexture2D>(AssetManager::GetTexture(texturePath), tileWidth, tileHeight));
		}

		tinyxml2::XMLElement* pTile = pRoot->FirstChildElement("tile");

		while (pTile)
		{
			int tileId = atoi(pTile->Attribute("id"));
			const char* probability = pTile->Attribute("probability");
			if (probability != nullptr)
			{
				tileset->SetTileProbability(tileId, atof(probability));
			}

			// TODO animated tiles
			/*tinyxml2::XMLElement* pAnimation = pTile->FirstChildElement("animation");
			if (pAnimation)
			{
				tinyxml2::XMLElement* pFrame = pAnimation->FirstChildElement("frame");

				uint32_t startFrame = std::numeric_limits<uint32_t>::max();
				uint32_t endFrame = 0;

				float duration = 0.0f;

				while (pFrame)
				{
					const char* tileid = pFrame->Attribute("tileid");
					if ((uint32_t)atoi(tileid) < startFrame)
						startFrame = atoi(tileid);
					if ((uint32_t)atoi(tileid) > endFrame)
						endFrame = atoi(tileid);
					pFrame->QueryFloatAttribute("duration", &duration);
					pFrame = pFrame->NextSiblingElement("frame");
				}
				tileset->AddAnimation("Unnamed Animation", startFrame, endFrame - startFrame + 1, duration);
			}*/
			pTile = pTile->NextSiblingElement("tile");
		}
		std::filesystem::path newFilePath = filepath;
		newFilePath.replace_extension(".tileset");
		tileset->SaveAs(destination / newFilePath.filename());
	}
	else
	{
		CLIENT_ERROR("Could not load tileset {0}. {1} on line {2}. File format must be xml!", filepath, doc.ErrorName(), doc.ErrorLineNum());
	}
}

void TiledImporter::ImportTileset(const std::filesystem::path& filepath, const std::filesystem::path& destination)
{
	LoadTileset(filepath, destination);
}

void TiledImporter::ImportTilemap(const std::filesystem::path& filepath, const std::filesystem::path& destination)
{
	tinyxml2::XMLDocument doc;

	s_Filepath = filepath;

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

		pRoot->QueryUnsignedAttribute("tilewidth", &s_TileWidth);
		pRoot->QueryUnsignedAttribute("tileheight", &s_TileHeight);

		tinyxml2::XMLElement* pTileSet = pRoot->FirstChildElement("tileset");

		std::filesystem::path fileDirectory = filepath;
		fileDirectory.remove_filename();

		while (pTileSet)
		{
			const char* tsxPath = pTileSet->Attribute("source");

			std::filesystem::path tilesetPath(fileDirectory / tsxPath);

			LoadTileset(tilesetPath, destination);

			tilesetPath.replace_extension(".tileset");

			Ref<Tileset> tileset = AssetManager::GetAsset<Tileset>(tilesetPath);
			if (tileset)
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

		tinyxml2::XMLElement* pImageLayer = pRoot->FirstChildElement("imagelayer");

		while (pImageLayer)
		{
			LoadImageLayer(pImageLayer);

			pImageLayer = pImageLayer->NextSiblingElement("imagelayer");
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
		CLIENT_ERROR("Could not load Tiled tilemap {0}. {1} on line {2}. File format must be xml!", filepath, doc.ErrorName(), doc.ErrorLineNum());
	}

	s_Tilesets.clear();
}