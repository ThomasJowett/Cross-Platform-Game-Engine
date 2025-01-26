#include "stdafx.h"
#include "TilemapComponent.h"
#include "Scene/SceneManager.h"

Vector2f TilemapComponent::IsoToWorld(uint32_t x, uint32_t y) const
{
	return Vector2f((float)((int)x - (int)y) / 2.0f, -(float)(x + y) / 4.0f);
}

Vector2f TilemapComponent::WorldToIso(Vector2f v) const
{
	return Vector2f((v.x - v.y * 2.0f), -(v.x + v.y * 2.0f));
}

Vector2f TilemapComponent::HexToWorld(uint32_t q, uint32_t r) const
{
	float hexWitdth = (float)tileWidth / (float)SceneManager::CurrentScene()->GetPixelsPerUnit();
	float hexHeight = (float)tileHeight / (float)SceneManager::CurrentScene()->GetPixelsPerUnit();

	float x = hexWitdth * 3.0f / 4.0f * q;
	float y = hexHeight * r;
	if (q % 2 == 1)
	{
		y += hexHeight * 0.5f;
	}
	return Vector2f(x, -y);
}

Vector2f TilemapComponent::WorldToHex(Vector2f v) const
{
	float hexWidth = (float)tileWidth / (float)SceneManager::CurrentScene()->GetPixelsPerUnit();
	float hexHeight = (float)tileHeight / (float)SceneManager::CurrentScene()->GetPixelsPerUnit();

	float approxQ = v.x / (hexWidth * 3.0f / 4.0f);
	int q = (int)std::round(approxQ);

	float yOffset = (q % 2 == 1) ? hexHeight * 0.5f : 0.0f;
	float approxR = (-v.y - yOffset) / hexHeight;
	int r = (int)std::round(approxR);
	return Vector2f((float)q, (float)r);
}

void TilemapComponent::Rebuild()
{
	mesh.reset();

	if (!tileset || !tileset->GetSubTexture())
		return;

	std::vector<Vertex> verticesList;
	std::vector<uint32_t> indicesList;



	size_t maxTileIndex = tileset->GetNumberOfTiles();

	if (orientation == Orientation::orthogonal)
	{
		// 0,0________ X
		//   |_|_|_|_|
		//   |_|_|_|_|
		//   |_|_|_|_|
		//   |_|_|_|_|
		//  Y

		Vector2f positions[4] = {
				{ 0.0f, 1.0f },
				{ 1.0f, 1.0f },
				{ 1.0f, 0.0f },
				{ 0.0f, 0.0f }
		};

		for (size_t i = 0; i < tilesHigh; i++)
		{
			for (size_t j = 0; j < tilesWide; j++)
			{
				if (tiles[i][j] == 0)
					continue;

				if (tiles[i][j] > maxTileIndex) {
					tiles[i][j] = 0;
					continue;
				}

				tileset->SetCurrentTile(tiles[i][j] - 1);
				const Vector2f* texCoords = tileset->GetSubTexture()->GetTextureCoordinates();

				for (size_t v = 0; v < 4; v++)
				{
					Vertex vertex;
					vertex.position = Vector3f((float)(j)+positions[v].x, -(float)(i)-positions[v].y, 0.0f);
					vertex.normal.z = 1.0f;
					vertex.tangent.x = 1.0f;
					vertex.texcoord = Vector2f(texCoords[v].x, texCoords[v].y);
					verticesList.push_back(vertex);
				}
			}
		}
	}
	else if (orientation == Orientation::isometric)
	{
		//   0,0
		//    /\
		//   /\/\
		// Y/\/\/\ X
		//  \/\/\/
		//   \/\/
		//    \/

		Vector2f positions[4] = {
				{ 0.0f, 0.0f },
				{ 1.0f, 0.0f },
				{ 1.0f, 1.0f },
				{ 0.0f, 1.0f }
		};

		for (uint32_t i = 0; i < tilesHigh; i++)
		{
			for (uint32_t j = 0; j < tilesWide; j++)
			{
				if (tiles[i][j] == 0)
					continue;

				if (tiles[i][j] > maxTileIndex) {
					tiles[i][j] = 0;
					continue;
				}

				tileset->SetCurrentTile(tiles[i][j] - 1);
				const Vector2f* texCoords = tileset->GetSubTexture()->GetTextureCoordinates();

				Vector2f isoCoords = IsoToWorld(j, i);

				for (uint32_t v = 0; v < 4; v++)
				{
					Vertex vertex;

					vertex.position.x = isoCoords.x + positions[v].x - 0.5f;
					vertex.position.y = isoCoords.y + positions[v].y - 0.5f;
					vertex.position.z = (i + j) * 0.0001f;

					vertex.normal.z = 1.0f;
					vertex.tangent.x = 1.0f;

					vertex.texcoord = Vector2f(texCoords[v].x, texCoords[v].y);

					verticesList.push_back(vertex);
				}
			}
		}
	}
	else if (orientation == Orientation::hexagonal)
	{
		Vector2f positions[4] = {
				{ 0.0f, 0.0f },
				{ 1.0f, 0.0f },
				{ 1.0f, 1.0f },
				{ 0.0f, 1.0f }
		};

		for (uint32_t r = 0; r < tilesHigh; r++)
		{
			for (uint32_t q = 0; q < tilesWide; q++)
			{
				if (tiles[r][q] == 0)
					continue;

				if (tiles[r][q] > maxTileIndex) {
					tiles[r][q] = 0;
					continue;
				}

				tileset->SetCurrentTile(tiles[r][q] - 1);
				const Vector2f* texCoords = tileset->GetSubTexture()->GetTextureCoordinates();

				int tileSize[2] = { (int)tileset->GetSubTexture()->GetSpriteWidth(), (int)tileset->GetSubTexture()->GetSpriteHeight() };

				float hexWitdth = (float)tileWidth / (float)SceneManager::CurrentScene()->GetPixelsPerUnit();
				float hexHeight = (float)tileSize[1] / (float)SceneManager::CurrentScene()->GetPixelsPerUnit();

				Vector2f center = HexToWorld(q, r);

				for (size_t v = 0; v < 4; v++)
				{
					Vertex vertex;
					vertex.position.x = center.x + positions[v].x - 0.5f;
					if (positions[v].y < 1.0f)
						vertex.position.y = center.y + positions[v].y - (hexHeight * 0.5f);
					else
						vertex.position.y = center.y + positions[v].y - 0.5f;
					vertex.position.z = r * 0.0001f;

					vertex.normal.z = 1.0f;
					vertex.tangent.x = 1.0f;
					vertex.texcoord = Vector2f(texCoords[v].x, texCoords[v].y);
					verticesList.push_back(vertex);
				}
			}
		}
	}

	if (verticesList.size() == 0)
		return;

	uint32_t index = 0;

	for (size_t i = 0; i < tilesHigh; i++)
	{
		for (size_t j = 0; j < tilesWide; j++)
		{
			indicesList.push_back(index);
			indicesList.push_back(index + 1);
			indicesList.push_back(index + 2);

			indicesList.push_back(index);
			indicesList.push_back(index + 2);
			indicesList.push_back(index + 3);

			index += 4;
		}
	}

	Ref<Material> material = CreateRef<Material>("Standard", tint);
	material->AddTexture(tileset->GetSubTexture()->GetTexture(), 0);
	material->SetTwoSided(true);
	material->SetTransparency(true);

	mesh = CreateRef<Mesh>(verticesList, indicesList, material);
}