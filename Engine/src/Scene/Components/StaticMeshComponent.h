#pragma once

#include "Core/core.h"
#include "Renderer/Mesh.h"

class StaticMeshComponent
{
public:
	StaticMeshComponent()
	{
	}

	~StaticMeshComponent()
	{
	}

private:
	Ref<Mesh> m_Mesh;
};
