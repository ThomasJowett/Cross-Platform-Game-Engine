#include "stdafx.h"
#include "Mesh.h"
#include "Scene/AssetManager.h"

/* ------------------------------------------------------------------------------------------------------------------ */

Mesh::Mesh(Ref<VertexArray> vertexArray)
	:m_VertexArray(vertexArray)
{
	m_Material = AssetManager::GetMaterial("_");
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