#include "stdafx.h"
#include "Mesh.h"

/* ------------------------------------------------------------------------------------------------------------------ */

Mesh::Mesh(Ref<VertexArray> vertexArray)
	:m_VertexArray(vertexArray)
{
	m_Material = CreateRef<Material>();
}

/* ------------------------------------------------------------------------------------------------------------------ */

Mesh::Mesh(Ref<VertexArray> vertexArray, Ref<Material> material)
	:m_VertexArray(vertexArray), m_Material(material)
{
}

/* ------------------------------------------------------------------------------------------------------------------ */

void Mesh::LoadModel(Ref<VertexArray> vertexArray)
{
	//m_Bounds = aabb;
	m_VertexArray = vertexArray;
}

/* ------------------------------------------------------------------------------------------------------------------ */