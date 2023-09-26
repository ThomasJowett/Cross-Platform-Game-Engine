#include "stdafx.h"
#include "TilemapComponent.h"

Vector2f TilemapComponent::IsoToWorld(uint32_t x, uint32_t y) const
{
	return Vector2f((float)((int)x - (int)y) / 2.0f, -(float)(x + y) / 4.0f);
}

Vector2f TilemapComponent::WorldToIso(Vector2f v) const
{
	return Vector2f((v.x - v.y * 2.0f), -(v.x + v.y * 2.0f));
}

void TilemapComponent::Rebuild()
{
	mesh.reset();

	if (!tileset || !tileset->GetSubTexture())
		return;

	std::vector<Vertex> verticesList;
	std::vector<uint32_t> indicesList;

	Vector2f positions[4] = {
					{ 0.0f, 1.0f },
					{ 1.0f, 1.0f },
					{ 1.0f, 0.0f },
					{ 0.0f, 0.0f }
	};

	size_t maxTileIndex = tileset->GetNumberOfTiles();

	if (orientation == Orientation::orthogonal)
	{
		// 0,0________ X
		//   |_|_|_|_|
		//   |_|_|_|_|
		//   |_|_|_|_|
		//   |_|_|_|_|
		//  Y

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

				for (uint32_t v = 0; v < 4; v++)
				{
					Vertex vertex;
					Vector2f isoCoords = IsoToWorld(j, i);

					vertex.position.x = isoCoords.x + positions[3 - v].x - 0.5f;
					vertex.position.y = isoCoords.y + positions[3 - v].y - 0.5f;
					vertex.position.z = (i + j) * 0.0001f;

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