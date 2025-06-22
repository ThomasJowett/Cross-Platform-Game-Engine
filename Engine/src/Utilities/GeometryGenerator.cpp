#include "stdafx.h"
#include "GeometryGenerator.h"

#include "math/Quaternion.h"
#include "Renderer/Mesh.h"
#include "Logging/Instrumentor.h"

/* ------------------------------------------------------------------------------------------------------------------ */

std::vector<float> GeometryGenerator::FlattenVertices(const std::vector<Vertex>& vertices)
{
	PROFILE_FUNCTION();
	std::vector<float> flatVertices;
	flatVertices.reserve(vertices.size() * 11);
	for (const auto& vertex : vertices)
	{
		flatVertices.push_back(vertex.position.x);
		flatVertices.push_back(vertex.position.y);
		flatVertices.push_back(vertex.position.z);
		flatVertices.push_back(vertex.normal.x);
		flatVertices.push_back(vertex.normal.y);
		flatVertices.push_back(vertex.normal.z);
		flatVertices.push_back(vertex.tangent.x);
		flatVertices.push_back(vertex.tangent.y);
		flatVertices.push_back(vertex.tangent.z);
		flatVertices.push_back(vertex.texcoord.x);
		flatVertices.push_back(vertex.texcoord.y);
	}
	return flatVertices;
}

std::vector<float> FlattenFullscreenVertices(const std::vector<FullScreenVertex>& vertices)
{
	PROFILE_FUNCTION();
	std::vector<float> data;
	data.reserve(vertices.size() * 5); // 3 (position) + 2 (uv)

	for (const auto& v : vertices)
	{
		data.push_back(v.position.x);
		data.push_back(v.position.y);
		data.push_back(v.position.z);
		data.push_back(v.texcoord.x);
		data.push_back(v.texcoord.y);
	}

	return data;
}

Ref<Mesh> GeometryGenerator::CreateCube(float width, float height, float depth)
{
	PROFILE_FUNCTION();

	float w2 = 0.5f * width;
	float h2 = 0.5f * height;
	float d2 = 0.5f * depth;

	std::vector<Vertex> vertices =
	{
		// Front
		Vertex({ Vector3f(-w2, -h2, d2), Vector3f(0.0f, 0.0f, +1.0f), Vector3f(1.0f, 0.0f, 0.0f), Vector2f(0.0f, 0.0f)}),
		Vertex({ Vector3f(+w2, -h2, d2), Vector3f(0.0f, 0.0f, +1.0f), Vector3f(1.0f, 0.0f, 0.0f), Vector2f(1.0f, 0.0f)}),
		Vertex({ Vector3f(+w2, +h2, d2), Vector3f(0.0f, 0.0f, +1.0f), Vector3f(1.0f, 0.0f, 0.0f), Vector2f(1.0f, 1.0f)}),
		Vertex({ Vector3f(-w2, +h2, d2), Vector3f(0.0f, 0.0f, +1.0f), Vector3f(1.0f, 0.0f, 0.0f), Vector2f(0.0f, 1.0f)}),
		// Back
		Vertex({ Vector3f(+w2, -h2, -d2), Vector3f(0.0f, 0.0f, -1.0f), Vector3f(-1.0f, 0.0f, 0.0f), Vector2f(0.0f, 0.0f)}),
		Vertex({ Vector3f(-w2, -h2, -d2), Vector3f(0.0f, 0.0f, -1.0f), Vector3f(-1.0f, 0.0f, 0.0f), Vector2f(1.0f, 0.0f)}),
		Vertex({ Vector3f(-w2, +h2, -d2), Vector3f(0.0f, 0.0f, -1.0f), Vector3f(-1.0f, 0.0f, 0.0f), Vector2f(1.0f, 1.0f)}),
		Vertex({ Vector3f(+w2, +h2, -d2), Vector3f(0.0f, 0.0f, -1.0f), Vector3f(-1.0f, 0.0f, 0.0f), Vector2f(0.0f, 1.0f)}),
		// Top
		Vertex({ Vector3f(-w2, +h2, +d2), Vector3f(0.0f, 1.0f, 0.0f), Vector3f(1.0f, 0.0f, 0.0f), Vector2f(0.0f, 0.0f)}),
		Vertex({ Vector3f(+w2, +h2, +d2), Vector3f(0.0f, 1.0f, 0.0f), Vector3f(1.0f, 0.0f, 0.0f), Vector2f(1.0f, 0.0f)}),
		Vertex({ Vector3f(+w2, +h2, -d2), Vector3f(0.0f, 1.0f, 0.0f), Vector3f(1.0f, 0.0f, 0.0f), Vector2f(1.0f, 1.0f)}),
		Vertex({ Vector3f(-w2, +h2, -d2), Vector3f(0.0f, 1.0f, 0.0f), Vector3f(1.0f, 0.0f, 0.0f), Vector2f(0.0f, 1.0f)}),
		// Bottom
		Vertex({ Vector3f(-w2, -h2, -d2), Vector3f(0.0f, -1.0f, 0.0f), Vector3f(-1.0f, 0.0f, 0.0f), Vector2f(0.0f, 0.0f)}),
		Vertex({ Vector3f(+w2, -h2, -d2), Vector3f(0.0f, -1.0f, 0.0f), Vector3f(-1.0f, 0.0f, 0.0f), Vector2f(1.0f, 0.0f)}),
		Vertex({ Vector3f(+w2, -h2, +d2), Vector3f(0.0f, -1.0f, 0.0f), Vector3f(-1.0f, 0.0f, 0.0f), Vector2f(1.0f, 1.0f)}),
		Vertex({ Vector3f(-w2, -h2, +d2), Vector3f(0.0f, -1.0f, 0.0f), Vector3f(-1.0f, 0.0f, 0.0f), Vector2f(0.0f, 1.0f)}),
		// Left
		Vertex({ Vector3f(-w2, -h2, -d2), Vector3f(-1.0f, 0.0f, 0.0f), Vector3f(0.0f, 0.0f, -1.0f), Vector2f(0.0f, 0.0f)}),
		Vertex({ Vector3f(-w2, -h2, +d2), Vector3f(-1.0f, 0.0f, 0.0f), Vector3f(0.0f, 0.0f, -1.0f), Vector2f(1.0f, 0.0f)}),
		Vertex({ Vector3f(-w2, +h2, +d2), Vector3f(-1.0f, 0.0f, 0.0f), Vector3f(0.0f, 0.0f, -1.0f), Vector2f(1.0f, 1.0f)}),
		Vertex({ Vector3f(-w2, +h2, -d2), Vector3f(-1.0f, 0.0f, 0.0f), Vector3f(0.0f, 0.0f, -1.0f), Vector2f(0.0f, 1.0f)}),
		// Right
		Vertex({ Vector3f(+w2, -h2, +d2), Vector3f(1.0f, 0.0f, 0.0f), Vector3f(0.0f, 0.0f, 1.0f), Vector2f(0.0f, 0.0f)}),
		Vertex({ Vector3f(+w2, -h2, -d2), Vector3f(1.0f, 0.0f, 0.0f), Vector3f(0.0f, 0.0f, 1.0f), Vector2f(1.0f, 0.0f)}),
		Vertex({ Vector3f(+w2, +h2, -d2), Vector3f(1.0f, 0.0f, 0.0f), Vector3f(0.0f, 0.0f, 1.0f), Vector2f(1.0f, 1.0f)}),
		Vertex({ Vector3f(+w2, +h2, +d2), Vector3f(1.0f, 0.0f, 0.0f), Vector3f(0.0f, 0.0f, 1.0f), Vector2f(0.0f, 1.0f)})
	};

	std::vector<uint32_t> indices =
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

	return CreateRef<Mesh>(vertices, indices, Material::GetDefaultMaterial(), s_StaticMeshLayout);
}

/* ------------------------------------------------------------------------------------------------------------------ */

Ref<Mesh> GeometryGenerator::CreateSphere(float radius, uint32_t longitudeLines, uint32_t latitudeLines)
{
	PROFILE_FUNCTION();

	if (longitudeLines < 3)
		longitudeLines = 3;
	if (latitudeLines < 3)
		latitudeLines = 3;

	std::vector<Vertex> vertices;

	float phiStep = (float)PI / (latitudeLines - 1);
	float thetaStep = 2.0f * (float)PI / longitudeLines;

	// Compute vertices for each latitude ring
	for (uint32_t i = 0; i < latitudeLines; ++i)
	{
		float phi = i * phiStep;

		// vertices of ring
		for (uint32_t j = 0; j <= longitudeLines; j++)
		{
			float theta = j * thetaStep;

			Vertex vertex;

			//positions
			//spherical to cartesian
			vertex.position.x = radius * sinf(phi) * cosf(theta);
			vertex.position.y = radius * cosf(phi);
			vertex.position.z = radius * sinf(phi) * sinf(theta);

			//normal
			vertex.normal = vertex.position.GetNormalized();

			//tangent
			vertex.tangent.x = -radius * sinf(phi) * sinf(theta);
			vertex.tangent.y = 0.0f;
			vertex.tangent.z = radius * sinf(phi) * cosf(theta);

			vertex.tangent.Normalize();

			//TexCoords
			vertex.texcoord.x = (1 - (theta / (float)PI)) / 2;
			vertex.texcoord.y = 1 - (phi / (float)PI);

			vertices.push_back(vertex);
		}
	}

	std::vector<uint32_t> indices;
	uint32_t k1, k2;
	for (uint32_t i = 0; i <= latitudeLines; i++)
	{
		k1 = i * (longitudeLines + 1);
		k2 = k1 + (longitudeLines + 1);

		for (uint32_t j = 0; j < longitudeLines; ++j, ++k1, ++k2)
		{
			if (i != 0)
			{
				indices.push_back(k1 + 1);
				indices.push_back(k2);
				indices.push_back(k1);
			}

			if (i != (latitudeLines - 2))
			{
				indices.push_back(k2 + 1);
				indices.push_back(k2);
				indices.push_back(k1 + 1);
			}
		}
	}

	return CreateRef<Mesh>(vertices, indices, Material::GetDefaultMaterial(), s_StaticMeshLayout);
}

/* ------------------------------------------------------------------------------------------------------------------ */

Ref<Mesh> GeometryGenerator::CreateGrid(float width, float length, uint32_t widthLines, uint32_t lengthLines, float tileU, float tileV)
{
	PROFILE_FUNCTION();

	if (widthLines < 2)
		widthLines = 2;
	if (lengthLines < 2)
		lengthLines = 2;

	uint32_t vertexCount = widthLines * lengthLines;
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

	std::vector<Vertex> vertices;
	vertices.resize(vertexCount);

	uint32_t k = 0;

	for (uint32_t i = 0; i < widthLines; ++i)
	{
		float z = halfDepth - i * dz;

		for (uint32_t j = 0; j < lengthLines; ++j)
		{
			float x = -halfWidth + j * dx;

			// position
			vertices[k].position.x = x;
			vertices[k].position.y = 0.0f;
			vertices[k].position.z = z;

			// normal
			vertices[k].normal.x = 0.0f;
			vertices[k].normal.y = 1.0f;
			vertices[k].normal.z = 0.0f;

			// tangent
			vertices[k].tangent.x = 1.0f;
			vertices[k].tangent.y = 0.0f;
			vertices[k].tangent.z = 0.0f;

			vertices[k].texcoord.x = j * du;
			vertices[k].texcoord.y = i * dv;

			k++;
		}
	}

	std::vector<uint32_t> indices(facecount * 3);

	k = 0;
	for (uint32_t i = 0; i < widthLines - 1; ++i)
	{
		for (uint32_t j = 0; j < lengthLines - 1; ++j)
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

	return CreateRef<Mesh>(vertices, indices, Material::GetDefaultMaterial(), s_StaticMeshLayout);
}

/* ------------------------------------------------------------------------------------------------------------------ */

Ref<Mesh> GeometryGenerator::CreateFullScreenQuad()
{
	PROFILE_FUNCTION();

	std::vector<Vertex> vertices = {
		Vertex({Vector3f(-1.0f, -1.0f, 0.0f),  Vector3f(0.0f, 0.0f, -1.0f),  Vector3f(1.0f, 0.0f, 0.0f),  Vector2f(0.0f, 1.0f)}),
		Vertex({Vector3f(-1.0f, +1.0f, 0.0f),  Vector3f(0.0f, 0.0f, -1.0f),  Vector3f(1.0f, 0.0f, 0.0f),  Vector2f(0.0f, 0.0f)}),
		Vertex({Vector3f(+1.0f, +1.0f, 0.0f),  Vector3f(0.0f, 0.0f, -1.0f),  Vector3f(1.0f, 0.0f, 0.0f),  Vector2f(1.0f, 0.0f)}),
		Vertex({Vector3f(+1.0f, -1.0f, 0.0f),  Vector3f(0.0f, 0.0f, -1.0f),  Vector3f(1.0f, 0.0f, 0.0f),  Vector2f(1.0f, 1.0f)})
	};

	std::vector<uint32_t> indices =
	{
		0,1,2,
		0,2,3
	};

	return CreateRef<Mesh>(vertices, indices, Material::GetDefaultMaterial(), s_FullscreenLayout);
}

/* ------------------------------------------------------------------------------------------------------------------ */

Ref<Mesh> GeometryGenerator::CreateCylinder(float bottomRadius, float topRadius, float height, uint32_t sliceCount, uint32_t stackCount)
{
	PROFILE_FUNCTION();

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

	std::vector<Vertex> vertices;

	//Compute the vertices for each stack ring starting at the bottom and moving up
	for (uint32_t i = 0; i < ringCount; i++)
	{
		float y = -0.5f * height + i * stackHeight;
		float r = bottomRadius + i * radiusStep;

		float dTheta = 2.0f * (float)PI / sliceCount;
		for (uint32_t j = 0; j <= sliceCount; j++)
		{
			Vertex vertex;
			float c = cosf(j * dTheta);
			float s = sinf(j * dTheta);

			//Position
			vertex.position = Vector3f(r * c, y, r * s);

			vertex.tangent = Vector3f(-s, 0.0f, c);

			float dr = bottomRadius - topRadius;
			Vector3f biTangent(dr * c, -height, dr * s);

			vertex.normal = Vector3f::Cross(vertex.tangent, biTangent).GetNormalized();

			//TexCoords
			vertex.texcoord = Vector2f(1.0f - (float)j / sliceCount, (float)i / stackCount);

			vertices.push_back(vertex);
		}
	}

	uint32_t ringVertexCount = sliceCount + 1;

	std::vector<uint32_t> indices;

	//compute the indices for each stack
	for (uint32_t i = 0; i < stackCount; i++)
	{
		for (uint32_t j = 0; j < sliceCount; j++)
		{
			indices.push_back(i * ringVertexCount + j);
			indices.push_back((i + 1) * ringVertexCount + j);
			indices.push_back((i + 1) * ringVertexCount + j + 1);

			indices.push_back(i * ringVertexCount + j);
			indices.push_back((i + 1) * ringVertexCount + j + 1);
			indices.push_back(i * ringVertexCount + j + 1);
		}
	}

	//build top cap
	uint32_t baseIndex = (uint32_t)(vertices.size());

	float y = 0.5f * height;
	float dTheta = 2.0f * (float)PI / sliceCount;

	for (uint32_t i = 0; i <= sliceCount; i++)
	{
		Vertex vertex;
		float x = topRadius * cosf(i * dTheta);
		float z = topRadius * sinf(i * dTheta);

		vertex.position = Vector3f(x, y, z);
		vertex.normal = Vector3f(0.0f, 1.0f, 0.0f);
		vertex.tangent = Vector3f(1.0f, 0.0f, 0.0f);
		vertex.texcoord = Vector2f(x / height + 0.5f, 1.0f - (z / height + 0.5f));

		vertices.push_back(vertex);
	}

	vertices.push_back(Vertex({ Vector3f(0.0f, y, 0.0f), Vector3f(0.0f, 1.0f, 0.0f), Vector3f(1.0f, 0.0f, 0.0f), Vector2f(0.5f, 0.5f) }));

	uint32_t centerIndex = (uint32_t)(vertices.size() - 1);

	for (uint32_t i = 0; i < sliceCount; i++)
	{
		indices.push_back(centerIndex);
		indices.push_back(baseIndex + i + 1);
		indices.push_back(baseIndex + i);
	}

	//build bottom cap
	baseIndex = (uint32_t)(vertices.size());
	y = -0.5f * height;

	//vertices of ring
	dTheta = 2.0f * (float)PI / sliceCount;
	for (uint32_t i = 0; i <= sliceCount; i++)
	{
		Vertex vertex;
		float x = bottomRadius * cosf(i * dTheta);
		float z = bottomRadius * sinf(i * dTheta);

		vertex.position = Vector3f(x, y, z);
		vertex.normal = Vector3f(0.0f, -1.0f, 0.0f);
		vertex.tangent = Vector3f(1.0f, 0.0f, 0.0f);
		vertex.texcoord = Vector2f(x / height + 0.5f, z / height + 0.5f);
		vertices.push_back(vertex);
	}

	vertices.push_back(Vertex({ Vector3f(0.0f, y, 0.0f), Vector3f(0.0f, -1.0f, 0.0f), Vector3f(1.0f, 0.0f, 0.0f), Vector2f(0.5f, 0.5f) }));

	centerIndex = (uint32_t)(vertices.size() - 1);

	for (uint32_t i = 0; i < sliceCount; i++)
	{
		indices.push_back(centerIndex);
		indices.push_back(baseIndex + i);
		indices.push_back(baseIndex + i + 1);
	}

	return CreateRef<Mesh>(vertices, indices, Material::GetDefaultMaterial(), s_StaticMeshLayout);
}

/* ------------------------------------------------------------------------------------------------------------------ */

Ref<Mesh> GeometryGenerator::CreateTorus(float diameter, float thickness, uint32_t segments)
{
	PROFILE_FUNCTION();

	if (segments < 3)
		segments = 3;

	uint32_t stride = segments + 1;

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	for (uint32_t i = 0; i <= segments; i++)
	{
		float u = 1.0f - float(i) / segments;
		float outerAngle = (i * (float)PI / segments - (float)(PI / 2.0f)) * 2;

		Matrix4x4 transform = Matrix4x4::RotateY(outerAngle) * Matrix4x4::Translate({ diameter / 2.0f, 0.0f, 0.0f });

		for (uint32_t j = 0; j <= segments; j++)
		{
			Vertex vertex;
			float v = 1 - (float)(j) / segments;

			float innerAngle = (float)(j * PI / segments + PI) * 2;
			float dx = cos(innerAngle);
			float dy = sin(innerAngle);

			vertex.normal = Vector3f(dx, dy, 0);
			vertex.tangent = Vector3f(dy, -dx, 0);
			vertex.position = vertex.normal * (thickness / 2.0f);

			vertex.position = transform * vertex.position;
			vertex.normal = (transform * vertex.normal).GetNormalized();
			vertex.tangent = (transform * vertex.tangent).GetNormalized();
			vertex.texcoord = Vector2f(u, v);

			vertices.push_back(vertex);

			//Indices
			uint32_t nextI = (i + 1) % stride;
			uint32_t nextJ = (j + 1) % stride;

			indices.push_back(nextI * stride + j);
			indices.push_back(i * stride + nextJ);
			indices.push_back(i * stride + j);

			indices.push_back(nextI * stride + j);
			indices.push_back(nextI * stride + nextJ);
			indices.push_back(i * stride + nextJ);
		}
	}

	return CreateRef<Mesh>(vertices, indices, Material::GetDefaultMaterial(), s_StaticMeshLayout);
}