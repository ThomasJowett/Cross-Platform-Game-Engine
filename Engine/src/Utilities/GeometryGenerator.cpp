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
	PROFILE_FUNCTION();

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
		+w2, -h2, -d2, 0.0f, 0.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		-w2, -h2, -d2, 0.0f, 0.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		-w2, +h2, -d2, 0.0f, 0.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		+w2, +h2, -d2, 0.0f, 0.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
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

Ref<VertexArray> GeometryGenerator::CreateSphere(float radius, uint32_t longitudeLines, uint32_t latitudeLines)
{
	PROFILE_FUNCTION();

	Ref<VertexArray> returnModel = VertexArray::Create();

	if (longitudeLines < 3)
		longitudeLines = 3;
	if (latitudeLines < 2)
		latitudeLines = 2;

	std::vector<float> verticesList;

	float phiStep = (float)PI / latitudeLines;
	float thetaStep = 2.0f * (float)PI / longitudeLines;

	// Compute vertices for each latitude ring
	for (uint32_t i = 0; i <= latitudeLines; ++i)
	{
		float phi = i * phiStep;

		// vertices of ring
		for (uint32_t j = 0; j <= longitudeLines; j++)
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
			verticesList.push_back((1 - (theta / (float)PI)) / 2);
			verticesList.push_back((1 - (phi / (float)PI)));
		}
	}


	std::vector<uint32_t> indicesList;

	for (uint32_t i = 0; i <= latitudeLines; i++)
	{
		for (uint32_t j = 0; j < longitudeLines; j++)
		{
			indicesList.push_back(i * longitudeLines + j);
			indicesList.push_back(i * longitudeLines + j + 1);
			indicesList.push_back((i + 1) * longitudeLines + j + 1);

			indicesList.push_back(i * longitudeLines + j);
			indicesList.push_back((i + 1) * longitudeLines + j + 1);
			indicesList.push_back((i + 1) * longitudeLines + j);
		}
	}

	Ref<VertexBuffer> vertexBuffer;
	vertexBuffer = VertexBuffer::Create(&verticesList[0], sizeof(float) * (uint32_t)verticesList.size());
	Ref<IndexBuffer> indexBuffer;
	indexBuffer = IndexBuffer::Create(&indicesList[0], (uint32_t)indicesList.size());

	vertexBuffer->SetLayout(layout);

	returnModel->AddVertexBuffer(vertexBuffer);
	returnModel->SetIndexBuffer(indexBuffer);

	return returnModel;
}

Ref<VertexArray> GeometryGenerator::CreateGrid(float width, float length, uint32_t widthLines, uint32_t lengthLines, float tileU, float tileV)
{
	PROFILE_FUNCTION();

	Ref<VertexArray> returnModel = VertexArray::Create();

	if (widthLines < 2)
		widthLines = 2;
	if (lengthLines < 2)
		lengthLines = 2;

	uint32_t vertexCount = widthLines * lengthLines; // 11 float per vertex
	uint32_t facecount = (widthLines - 1) * (lengthLines - 1) * 2;

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

	uint32_t k = 0;

	for (uint32_t i = 0; i < widthLines; ++i)
	{
		float z = halfDepth - i * dz;

		for (uint32_t j = 0; j < lengthLines; ++j)
		{
			float x = -halfWidth + j * dx;

			// position
			vertices[k] = x;
			vertices[k + 1] = 0.0f;
			vertices[k + 2] = z;

			// normal
			vertices[k + 3] = 0.0f;
			vertices[k + 4] = 1.0f;
			vertices[k + 5] = 0.0f;

			// tangent
			vertices[k + 6] = 1.0f;
			vertices[k + 7] = 0.0f;
			vertices[k + 8] = 0.0f;

			vertices[k + 9] = j * du;
			vertices[k + 10] = i * dv;

			k += 11;
		}
	}

	uint32_t* indices = new uint32_t[facecount * 3];

	k = 0;
	for (uint32_t i = 0; i < lengthLines - 1; ++i)
	{
		for (uint32_t j = 0; j < widthLines - 1; ++j)
		{
			indices[k] = i * lengthLines + j;
			indices[k + 1] = i * lengthLines + j + 1;
			indices[k + 2] = (i + 1) * lengthLines + j;

			indices[k + 3] = (i + 1) * lengthLines + j;
			indices[k + 4] = i * lengthLines + j + 1;
			indices[k + 5] = (i + 1) * lengthLines + j + 1;

			k += 6;
		}
	}

	Ref<VertexBuffer> vertexBuffer;
	vertexBuffer = VertexBuffer::Create(vertices, sizeof(float) * vertexCount * 11);
	Ref<IndexBuffer> indexBuffer;
	indexBuffer = IndexBuffer::Create(indices, (uint32_t)facecount * 3);

	vertexBuffer->SetLayout(layout);

	returnModel->AddVertexBuffer(vertexBuffer);
	returnModel->SetIndexBuffer(indexBuffer);

	delete[] indices;
	delete[] vertices;

	return returnModel;
}

Ref<VertexArray> GeometryGenerator::CreateFullScreenQuad()
{
	PROFILE_FUNCTION();

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

Ref<VertexArray> GeometryGenerator::CreateCylinder(float bottomRadius, float topRadius, float height, uint32_t sliceCount, uint32_t stackCount)
{
	PROFILE_FUNCTION();

	Ref<VertexArray> returnModel = VertexArray::Create();

	if (sliceCount < 3)
		sliceCount = 3;
	if (stackCount < 1)
		stackCount = 1;

	if (bottomRadius < 0.0f)
		bottomRadius = 0.0f;
	if (topRadius < 0.0f)
		topRadius = 0.0f;

	CORE_ASSERT(!(bottomRadius == 0.0f && topRadius == 0.0f), "The cylinder cannot have a radius of 0");

	//Build Stacks
	float stackHeight = height / stackCount;

	//Amount to increment radius as we move up each stack level from bottom to top
	float radiusStep = (topRadius - bottomRadius) / stackCount;

	uint32_t ringCount = stackCount + 1;

	std::vector<float> verticesList;

	//Compute the vertices for each stack ring starting at the bottom and moving up
	for (uint32_t i = 0; i < ringCount; i++)
	{
		float y = -0.5f * height + i * stackHeight;
		float r = bottomRadius + i * radiusStep;

		float dTheta = 2.0f * (float)PI / sliceCount;
		for (uint32_t j = 0; j <= sliceCount; j++)
		{
			float c = cosf(j * dTheta);
			float s = sinf(j * dTheta);

			//Position
			verticesList.push_back(r * c);
			verticesList.push_back(y);
			verticesList.push_back(r * s);

			Vector3f tangent(-s, 0.0f, c);

			float dr = bottomRadius - topRadius;
			Vector3f biTangent(dr * c, -height, dr * s);

			Vector3f normal = Vector3f::Cross(tangent, biTangent).GetNormalized();

			//Normal
			verticesList.push_back(normal.x);
			verticesList.push_back(normal.y);
			verticesList.push_back(normal.z);

			//Tangent
			verticesList.push_back(tangent.x);
			verticesList.push_back(tangent.y);
			verticesList.push_back(tangent.z);

			//TexCoords
			verticesList.push_back(1.0f - (float)j / sliceCount);
			verticesList.push_back((float)i / stackCount);
		}
	}

	uint32_t ringVertexCount = sliceCount + 1;

	std::vector<uint32_t> indicesList;

	//compute the indices for each stack
	for (uint32_t i = 0; i < stackCount; i++)
	{
		for (uint32_t j = 0; j < sliceCount; j++)
		{
			indicesList.push_back(i * ringVertexCount + j);
			indicesList.push_back((i + 1) * ringVertexCount + j);
			indicesList.push_back((i + 1) * ringVertexCount + j + 1);

			indicesList.push_back(i * ringVertexCount + j);
			indicesList.push_back((i + 1) * ringVertexCount + j + 1);
			indicesList.push_back(i * ringVertexCount + j + 1);
		}
	}

	//build top cap
	uint32_t baseIndex = (uint32_t)(verticesList.size() / 11.0f);

	float y = 0.5f * height;
	float dTheta = 2.0f * (float)PI / sliceCount;

	for (uint32_t i = 0; i <= sliceCount; i++)
	{
		float x = topRadius * cosf(i * dTheta);
		float z = topRadius * sinf(i * dTheta);

		//Position
		verticesList.push_back(x);
		verticesList.push_back(y);
		verticesList.push_back(z);

		//Normal
		verticesList.push_back(0.0f);
		verticesList.push_back(1.0f);
		verticesList.push_back(0.0f);

		//Tangent
		verticesList.push_back(1.0f);
		verticesList.push_back(0.0f);
		verticesList.push_back(0.0f);

		//TexCoords
		verticesList.push_back((x / height + 0.5f));
		verticesList.push_back(1.0f - (z / height + 0.5f));

	}

	//Position
	verticesList.push_back(0.0f);
	verticesList.push_back(y);
	verticesList.push_back(0.0f);

	//Normal
	verticesList.push_back(0.0f);
	verticesList.push_back(1.0f);
	verticesList.push_back(0.0f);

	//Tangent
	verticesList.push_back(1.0f);
	verticesList.push_back(0.0f);
	verticesList.push_back(0.0f);

	//TexCoords
	verticesList.push_back(0.5f);
	verticesList.push_back(0.5f);

	uint32_t centerIndex = (uint32_t)(verticesList.size() / 11.0f - 1);

	for (uint32_t i = 0; i < sliceCount; i++)
	{
		indicesList.push_back(centerIndex);
		indicesList.push_back(baseIndex + i + 1);
		indicesList.push_back(baseIndex + i);
	}

	//build bottom cap
	baseIndex = (uint32_t)(verticesList.size() / 11.0f);
	y = -0.5f * height;

	//vertices of ring
	dTheta = 2.0f * (float)PI / sliceCount;
	for (uint32_t i = 0; i <= sliceCount; i++)
	{
		float x = bottomRadius * cosf(i * dTheta);
		float z = bottomRadius * sinf(i * dTheta);

		//Position
		verticesList.push_back(x);
		verticesList.push_back(y);
		verticesList.push_back(z);

		//Normal
		verticesList.push_back(0.0f);
		verticesList.push_back(-1.0f);
		verticesList.push_back(0.0f);

		//Tangent
		verticesList.push_back(1.0f);
		verticesList.push_back(0.0f);
		verticesList.push_back(0.0f);

		//TexCoords
		verticesList.push_back(x / height + 0.5f);
		verticesList.push_back(z / height + 0.5f);
	}

	//Position
	verticesList.push_back(0.0f);
	verticesList.push_back(y);
	verticesList.push_back(0.0f);

	//Normal
	verticesList.push_back(0.0f);
	verticesList.push_back(-1.0f);
	verticesList.push_back(0.0f);

	//Tangent
	verticesList.push_back(1.0f);
	verticesList.push_back(0.0f);
	verticesList.push_back(0.0f);

	//TexCoords
	verticesList.push_back(0.5f);
	verticesList.push_back(0.5f);

	centerIndex = (uint32_t)(verticesList.size() / 11.0f - 1);

	for (uint32_t i = 0; i < sliceCount; i++)
	{
		indicesList.push_back(centerIndex);
		indicesList.push_back(baseIndex + i);
		indicesList.push_back(baseIndex + i + 1);
	}

	Ref<VertexBuffer> vertexBuffer;
	vertexBuffer = VertexBuffer::Create(&verticesList[0], sizeof(float) * (uint32_t)verticesList.size());
	Ref<IndexBuffer> indexBuffer;
	indexBuffer = IndexBuffer::Create(&indicesList[0], (uint32_t)indicesList.size());

	vertexBuffer->SetLayout(layout);

	returnModel->AddVertexBuffer(vertexBuffer);
	returnModel->SetIndexBuffer(indexBuffer);

	return returnModel;
}

Ref<VertexArray> GeometryGenerator::CreateTorus(float diameter, float thickness, uint32_t segments)
{
	PROFILE_FUNCTION();

	Ref<VertexArray> returnModel = VertexArray::Create();

	if (segments < 3)
		segments = 3;

	uint32_t stride = segments + 1;

	std::vector<float> verticesList;
	std::vector<uint32_t> indicesList;

	for (uint32_t i = 0; i <= segments; i++)
	{
		float u = 1.0f - float(i) / segments;
		float outerAngle = (i * (float)PI / segments - (float)(PI / 2.0f)) * 2;

		Matrix4x4 transform = Matrix4x4::RotateY(outerAngle) * Matrix4x4::Translate({ diameter / 2.0f, 0.0f, 0.0f });

		for (uint32_t j = 0; j <= segments; j++)
		{
			float v = 1 - (float)(j) / segments;

			float innerAngle = (float)(j * PI / segments + PI) * 2;
			float dx = cos(innerAngle);
			float dy = sin(innerAngle);

			Vector3f normal(dx, dy, 0);
			Vector3f tangent(dy, -dx, 0);
			Vector3f position = normal * (thickness / 2.0f);

			position = transform * position;
			normal = (transform * normal).GetNormalized();
			tangent = (transform * tangent).GetNormalized();

			//Position
			verticesList.push_back(position.x);
			verticesList.push_back(position.y);
			verticesList.push_back(position.z);

			//Normal
			verticesList.push_back(normal.x);
			verticesList.push_back(normal.y);
			verticesList.push_back(normal.z);

			//Tangent
			verticesList.push_back(tangent.x);
			verticesList.push_back(tangent.y);
			verticesList.push_back(tangent.z);

			//TexCoord
			verticesList.push_back(u);
			verticesList.push_back(v);

			//Indices
			uint32_t nextI = (i + 1) % stride;
			uint32_t nextJ = (j + 1) % stride;

			indicesList.push_back(nextI * stride + j);
			indicesList.push_back(i * stride + nextJ);
			indicesList.push_back(i * stride + j);

			indicesList.push_back(nextI * stride + j);
			indicesList.push_back(nextI * stride + nextJ);
			indicesList.push_back(i * stride + nextJ);
		}
	}

	Ref<VertexBuffer> vertexBuffer;
	vertexBuffer = VertexBuffer::Create(&verticesList[0], sizeof(float) * (uint32_t)verticesList.size());
	Ref<IndexBuffer> indexBuffer;
	indexBuffer = IndexBuffer::Create(&indicesList[0], (uint32_t)indicesList.size());

	vertexBuffer->SetLayout(layout);

	returnModel->AddVertexBuffer(vertexBuffer);
	returnModel->SetIndexBuffer(indexBuffer);

	return returnModel;
}
