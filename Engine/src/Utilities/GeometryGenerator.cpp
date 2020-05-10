#include "stdafx.h"
#include "GeometryGenerator.h"

#include "math/Quaternion.h"

// All geometry return with this layout so that it can be used in standard shaders
static BufferLayout layout = {
		{ShaderDataType::Float3, "a_Position"},
		{ShaderDataType::Float3, "a_Normal"},
		{ShaderDataType::Float3, "a_Tangent"},
		{ShaderDataType::Float2, "a_TexCoord"}
};

Ref<VertexArray> GeometryGenerator::CreateCube(float width, float height, float depth)
{
	Ref<VertexArray> returnModel = VertexArray::Create();

	float w2 = 0.5f * width;
	float h2 = 0.5f * height;
	float d2 = 0.5f * depth;

	float vertices[] =
	{
		// Front
		-w2, -h2, d2, 0.0f, 0.0f, +1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		+w2, -h2, d2, 0.0f, 0.0f, +1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		+w2, +h2, d2, 0.0f, 0.0f, +1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		-w2, +h2, d2, 0.0f, 0.0f, +1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		// Back
		-w2, -h2, -d2, 0.0f, 0.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		+w2, -h2, -d2, 0.0f, 0.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		+w2, +h2, -d2, 0.0f, 0.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		-w2, +h2, -d2, 0.0f, 0.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		// Top
		-w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		+w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		+w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		-w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		// Bottom
		-w2, -h2, -d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		+w2, -h2, -d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		+w2, -h2, +d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		-w2, -h2, +d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		// Left
		-w2, -h2, -d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
		-w2, -h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
		-w2, +h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
		-w2, +h2, -d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
		// Right
		+w2, -h2, +d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		+w2, -h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
		+w2, +h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		+w2, +h2, +d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f
	};

	uint32_t indices[] =
	{
		0, 1, 2,
		0, 2, 3,

		4, 5, 6,
		4, 6, 7,

		8, 9, 10,
		8, 10, 11,

		12, 13, 14,
		12, 14, 15,

		16, 17, 18,
		16, 18, 19,

		20, 21, 22,
		20, 22, 23
	};

	Ref<VertexBuffer> vertexBuffer;
	vertexBuffer = VertexBuffer::Create(vertices, sizeof(vertices));
	Ref<IndexBuffer> indexBuffer;
	indexBuffer = IndexBuffer::Create(indices, 36);

	vertexBuffer->SetLayout(layout);

	returnModel->AddVertexBuffer(vertexBuffer);
	returnModel->SetIndexBuffer(indexBuffer);

	return returnModel;
}

Ref<VertexArray> GeometryGenerator::CreateSphere(float radius, unsigned int longitudeLines, unsigned int latitudeLines)
{
	Ref<VertexArray> returnModel = VertexArray::Create();

	if (longitudeLines < 3)
		longitudeLines = 3;
	if (latitudeLines < 2)
		latitudeLines = 2;

	std::vector<float> verticesList;

	float phiStep = (float)PI / latitudeLines;
	float thetaStep = 2.0f * (float)PI / longitudeLines;

	// Compute vertices for each latitude ring
	for (unsigned int i = 0; i <= latitudeLines; ++i)
	{
		float phi = i * phiStep;

		// vertices of ring
		for (unsigned int j = 0; j <= longitudeLines; j++)
		{
			float theta = j * thetaStep;

			//positions
			Vector3f position;
			//spherical to cartesian
			position.x = radius * sinf(phi) * cosf(theta);
			position.y = radius * cosf(phi);
			position.z = radius * sinf(phi) * sinf(theta);

			verticesList.push_back(position.x);
			verticesList.push_back(position.y);
			verticesList.push_back(position.z);

			//normal
			Vector3f normal = position.GetNormalized();

			verticesList.push_back(normal.x);
			verticesList.push_back(normal.y);
			verticesList.push_back(normal.z);

			//tangent
			Vector3f tangent;
			tangent.x = -radius * sinf(phi) * sinf(theta);
			tangent.y = 0.0f;
			tangent.z = radius * sinf(phi) * cosf(theta);

			verticesList.push_back(tangent.x);
			verticesList.push_back(tangent.y);
			verticesList.push_back(tangent.z);

			//TexCoords
			verticesList.push_back((1 - (theta / (float)PI))/2);
			verticesList.push_back((1- (phi / (float)PI)));
		}
	}


	std::vector<unsigned int> indicesList;

	for (unsigned int i = 0; i <= latitudeLines; i++)
	{
		for (unsigned int j = 0; j < longitudeLines; j++)
		{
			indicesList.push_back(i * longitudeLines + j);
			indicesList.push_back(i * longitudeLines + j + 1);
			indicesList.push_back((i + 1) * longitudeLines + j+1);

			indicesList.push_back(i * longitudeLines + j);
			indicesList.push_back((i + 1) * longitudeLines + j + 1);
			indicesList.push_back((i+1) * longitudeLines + j);
		}
	}

	float* vertices = new float[verticesList.size()];
	for (unsigned int i = 0; i < verticesList.size(); ++i)
	{
		vertices[i] = verticesList[i];
	}

	unsigned int* indices = new unsigned int[indicesList.size()];
	for (unsigned int i = 0; i < indicesList.size(); ++i)
	{
		indices[i] = indicesList[i];
	}

	Ref<VertexBuffer> vertexBuffer;
	vertexBuffer = VertexBuffer::Create(vertices, sizeof(float) * (uint32_t)verticesList.size());
	Ref<IndexBuffer> indexBuffer;
	indexBuffer = IndexBuffer::Create(indices, (uint32_t)indicesList.size());

	vertexBuffer->SetLayout(layout);

	returnModel->AddVertexBuffer(vertexBuffer);
	returnModel->SetIndexBuffer(indexBuffer);

	return returnModel;
}

Ref<VertexArray> GeometryGenerator::CreateGrid(float width, float length, unsigned int widthLines, unsigned int lengthLines, float tileU, float tileV)
{
	Ref<VertexArray> returnModel = VertexArray::Create();

	if (widthLines < 2)
		widthLines = 2;
	if (lengthLines < 2)
		lengthLines = 2;

	int vertexCount = widthLines * lengthLines; // 11 float per vertex
	int facecount = (widthLines - 1)*(lengthLines - 1) * 2;

	//
	// Create the vertices
	//

	float halfWidth = 0.5f * width;
	float halfDepth = 0.5f * length;

	float dx = width / (lengthLines - 1);
	float dz = length / (widthLines - 1);

	float du = tileU / (lengthLines - 1);
	float dv = tileV / (widthLines - 1);

	float* vertices = new float[vertexCount * 11];

	int k = 0;

	for (unsigned int i = 0; i < widthLines; ++i)
	{
		float z = halfDepth - i * dz;

		for (unsigned int j = 0; j < lengthLines; ++j)
		{
			float x = -halfWidth + j * dx;

			// position
			vertices[k] = x;
			vertices[k+1] = 0.0f;
			vertices[k+2] = z;

			// normal
			vertices[k+3] = 0.0f;
			vertices[k+4] = 1.0f;
			vertices[k+5] = 0.0f;

			// tangent
			vertices[ k+6] = 1.0f;
			vertices[ k+7] = 0.0f;
			vertices[ k+8] = 0.0f;

			vertices[k+9] = j * du;
			vertices[k+10] = i * dv;

			k += 11;
		}
	}

	unsigned int* indices = new unsigned int[facecount * 3];

	k = 0;
	for (unsigned int i = 0; i < lengthLines - 1; ++i)
	{
		for (unsigned int j = 0; j < widthLines - 1; ++j)
		{
			indices[k] = i * lengthLines + j;
			indices[k + 1] = i * lengthLines + j + 1;
			indices[k + 2] = (i + 1)*lengthLines + j;
			 
			indices[k + 3] = (i + 1) * lengthLines + j;
			indices[k + 4] = i * lengthLines + j + 1;
			indices[k + 5] = (i + 1) * lengthLines + j + 1;

			k += 6;
		}
	}

	Ref<VertexBuffer> vertexBuffer;
	vertexBuffer = VertexBuffer::Create(vertices, sizeof(float) * vertexCount * 11);
	Ref<IndexBuffer> indexBuffer;
	indexBuffer = IndexBuffer::Create(indices, (uint32_t)facecount*3);

	vertexBuffer->SetLayout(layout);

	returnModel->AddVertexBuffer(vertexBuffer);
	returnModel->SetIndexBuffer(indexBuffer);

	return returnModel;
}

Ref<VertexArray> GeometryGenerator::CreateFullScreenQuad()
{
	Ref<VertexArray> returnModel = VertexArray::Create();

	float vertices[] =
	{
		-1.0f, -1.0f, 0.0f,  0.0f, 0.0f, -1.0f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
		-1.0f, +1.0f, 0.0f,  0.0f, 0.0f, -1.0f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
		+1.0f, +1.0f, 0.0f,  0.0f, 0.0f, -1.0f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f
		+ 1.0f, -1.0f, 0.0f,  0.0f, 0.0f, -1.0f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f
	};

	uint32_t indices[] =
	{
		0,1,2,
		0,2,3
	};

	Ref<VertexBuffer> vertexBuffer;
	vertexBuffer = VertexBuffer::Create(vertices, sizeof(float) * 44);
	Ref<IndexBuffer> indexBuffer;
	indexBuffer = IndexBuffer::Create(indices, (uint32_t)6);

	vertexBuffer->SetLayout(layout);

	returnModel->AddVertexBuffer(vertexBuffer);
	returnModel->SetIndexBuffer(indexBuffer);

	return returnModel;
}
