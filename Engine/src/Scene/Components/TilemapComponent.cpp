#include "TilemapComponent.h"
#include "Scene/SceneManager.h"
#include "Utilities/GeometryGenerator.h"

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
	if (!tileset || !tileset->GetSubTexture())
	{
		mesh.reset();
		return;
	}

	if (!rebuildState)
	{
		rebuildState = CreateRef<RebuildState>();
	}

	// currentRebuildId is also read (under the same lock) by an in-flight background thread's
	// own supersession check below - incrementing it here without the lock would be a data
	// race against that read.
	uint32_t rid;
	{
		std::lock_guard<std::mutex> lock(rebuildState->mutex);
		rid = ++rebuildState->currentRebuildId;
	}

	std::vector<std::vector<uint32_t>> tilesCopy = tiles;
	uint32_t tw = tilesWide;
	uint32_t th = tilesHigh;
	uint32_t tWidth = tileWidth;
	uint32_t tHeight = tileHeight;
	Orientation orient = orientation;
	Colour col = tint;

	size_t maxTileIndex = tileset->GetNumberOfTiles();
	std::vector<std::array<Vector2f, 4>> tileCoords(maxTileIndex);
	for (size_t i = 0; i < maxTileIndex; i++)
	{
		tileset->SetCurrentTile(static_cast<uint32_t>(i));
		const Vector2f* texCoords = tileset->GetSubTexture()->GetTextureCoordinates();
		for (size_t v = 0; v < 4; v++)
		{
			tileCoords[i][v] = texCoords[v];
		}
	}

	uint32_t spriteHeight = tileset->GetSubTexture()->GetSpriteHeight();
	uint32_t pixelsPerUnit = SceneManager::CurrentScene() ? SceneManager::CurrentScene()->GetPixelsPerUnit() : 100;
	Ref<Texture2D> tilesetTex = tileset->GetSubTexture()->GetTexture();

	std::weak_ptr<RebuildState> weakState = rebuildState;

	std::thread([
		weakState, rid, tilesCopy, tw, th, tWidth, tHeight, orient, col,
		maxTileIndex, tileCoords, spriteHeight, pixelsPerUnit, tilesetTex
	]() {
		std::vector<Vertex> verticesList;
		std::vector<uint32_t> indicesList;

		if (orient == Orientation::orthogonal)
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

			for (size_t i = 0; i < th; i++)
			{
				for (size_t j = 0; j < tw; j++)
				{
					if (i >= tilesCopy.size() || j >= tilesCopy[i].size() || tilesCopy[i][j] == 0)
						continue;

					if (tilesCopy[i][j] > maxTileIndex) {
						continue;
					}

					const auto& texCoords = tileCoords[tilesCopy[i][j] - 1];

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
		else if (orient == Orientation::isometric)
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

			auto isoToWorld = [](uint32_t x, uint32_t y) -> Vector2f {
				return Vector2f((float)((int)x - (int)y) / 2.0f, -(float)(x + y) / 4.0f);
			};

			for (uint32_t i = 0; i < th; i++)
			{
				for (uint32_t j = 0; j < tw; j++)
				{
					if (i >= tilesCopy.size() || j >= tilesCopy[i].size() || tilesCopy[i][j] == 0)
						continue;

					if (tilesCopy[i][j] > maxTileIndex) {
						continue;
					}

					const auto& texCoords = tileCoords[tilesCopy[i][j] - 1];

					Vector2f isoCoords = isoToWorld(j, i);

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
		else if (orient == Orientation::hexagonal)
		{
			Vector2f positions[4] = {
					{ 0.0f, 0.0f },
					{ 1.0f, 0.0f },
					{ 1.0f, 1.0f },
					{ 0.0f, 1.0f }
			};

			auto hexToWorld = [tWidth, tHeight, pixelsPerUnit](uint32_t q, uint32_t r) -> Vector2f {
				float hexWidth = (float)tWidth / (float)pixelsPerUnit;
				float hexHeight = (float)tHeight / (float)pixelsPerUnit;

				float x = hexWidth * 3.0f / 4.0f * q;
				float y = hexHeight * r;
				if (q % 2 == 1)
				{
					y += hexHeight * 0.5f;
				}
				return Vector2f(x, -y);
			};

			float hexHeight = (float)spriteHeight / (float)pixelsPerUnit;
			float hexWidth = (float)tWidth / (float)pixelsPerUnit;

			for (uint32_t r = 0; r < th; r++)
			{
				for (uint32_t q = 0; q < tw; q++)
				{
					if (r >= tilesCopy.size() || q >= tilesCopy[r].size() || tilesCopy[r][q] == 0)
						continue;

					if (tilesCopy[r][q] > maxTileIndex) {
						continue;
					}

					const auto& texCoords = tileCoords[tilesCopy[r][q] - 1];

					Vector2f center = hexToWorld(q, r);

					for (size_t v = 0; v < 4; v++)
					{
						Vertex vertex;
						vertex.position.x = center.x + (positions[v].x - 0.5f) * hexWidth;
						vertex.position.y = center.y + (positions[v].y - 0.5f) * hexHeight;
						vertex.position.z = r * 0.0001f;

						vertex.normal.z = 1.0f;
						vertex.tangent.x = 1.0f;
						vertex.texcoord = Vector2f(texCoords[v].x, texCoords[v].y);
						verticesList.push_back(vertex);
					}
				}
			}
		}

		// One quad (4 vertices) per non-empty tile actually pushed above - looping over the tile
		// grid again here (instead of verticesList.size()) would drift out of sync whenever any
		// tile was skipped (empty, or an out-of-range index reset to 0), producing indices past
		// the end of the vertex buffer.
		indicesList.reserve((verticesList.size() / 4) * 6);
		for (uint32_t index = 0; index < (uint32_t)verticesList.size(); index += 4)
		{
			indicesList.push_back(index);
			indicesList.push_back(index + 1);
			indicesList.push_back(index + 2);

			indicesList.push_back(index);
			indicesList.push_back(index + 2);
			indicesList.push_back(index + 3);
		}

		if (auto state = weakState.lock())
		{
			std::lock_guard<std::mutex> lock(state->mutex);
			if (rid >= state->currentRebuildId)
			{
				state->result.vertices = std::move(verticesList);
				state->result.indices = std::move(indicesList);
				state->result.texture = tilesetTex;
				state->result.tint = col;
				state->result.rebuildId = rid;
				state->hasResult = true;
			}
		}
	}).detach();
}

void TilemapComponent::UpdateRebuild()
{
	if (!rebuildState)
		return;

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	Ref<Texture2D> texture;
	Colour tintColor;
	bool hasNewResult = false;

	{
		std::lock_guard<std::mutex> lock(rebuildState->mutex);
		if (rebuildState->hasResult)
		{
			vertices = std::move(rebuildState->result.vertices);
			indices = std::move(rebuildState->result.indices);
			texture = std::move(rebuildState->result.texture);
			tintColor = rebuildState->result.tint;
			rebuildState->hasResult = false;
			hasNewResult = true;
		}
	}

	if (hasNewResult)
	{
		if (vertices.empty())
		{
			mesh.reset();
			return;
		}

		Ref<Material> material = CreateRef<Material>("Standard", tintColor);
		material->AddTexture(texture, 0);
		material->SetTwoSided(true);
		material->SetTransparency(true);

		mesh = CreateRef<Mesh>(GeometryGenerator::FlattenVertices(vertices), indices, material, s_StaticMeshLayout);
	}
}
