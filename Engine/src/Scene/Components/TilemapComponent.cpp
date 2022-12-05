#include "stdafx.h"
#include "TilemapComponent.h"

void TilemapComponent::Rebuild()
{
	vertexArray.reset();
	material.reset();

	if (!tileset)
		return;

	std::vector<float> verticesList;
	std::vector<uint32_t> indicesList;

	Vector2f positions[4] = {
				{ 0.0f, 1.0f },
				{ 1.0f, 1.0f },
				{ 1.0f, 0.0f },
				{ 0.0f, 0.0f }
	};

	uint32_t index = 0;

	for (size_t i = 0; i < tilesHigh; i++)
	{
		for (size_t j = 0; j < tilesWide; j++)
		{
			if (tiles[i][j] == 0)
				continue;

			tileset->SetCurrentTile(tiles[i][j] - 1);
			const Vector2f* texCoords = tileset->GetSubTexture()->GetTextureCoordinates();

			for (size_t v = 0; v < 4; v++)
			{
				// Position
				verticesList.push_back((float)(j) + positions[v].x);
				verticesList.push_back(-(float)(i) - positions[v].y);
				verticesList.push_back(0.0f);

				// TexCoord
				verticesList.push_back(texCoords[v].x);
				verticesList.push_back(texCoords[v].y);
			}

			
		}
	}

	if (verticesList.size() == 0)
		return;

	for (size_t i = 0; i < tilesHigh - 1; i++)
	{
		for (size_t j = 0; j < tilesWide - 1; j++)
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

	vertexArray = VertexArray::Create();

	Ref<VertexBuffer> vertexBuffer;
	vertexBuffer = VertexBuffer::Create(&verticesList[0], sizeof(float) * (uint32_t)verticesList.size());
	Ref<IndexBuffer> indexBuffer;
	indexBuffer = IndexBuffer::Create(&indicesList[0], (uint32_t)indicesList.size());

	vertexBuffer->SetLayout({
		{ShaderDataType::Float3, "a_Position"},
		{ShaderDataType::Float2, "a_Texcoord"},
		});

	vertexArray->AddVertexBuffer(vertexBuffer);
	vertexArray->SetIndexBuffer(indexBuffer);

	material = CreateRef<Material>("Renderer2D_Tilemap", tint);
	material->AddTexture(tileset->GetSubTexture()->GetTexture(), 0);
	material->SetTwoSided(true);
}
