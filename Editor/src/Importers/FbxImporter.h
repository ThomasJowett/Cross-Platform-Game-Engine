#pragma once

#include "ofbx.h"

#include "Renderer/VertexArray.h"

namespace Importer
{
	class Fbx
	{
	public:
		static Ref<VertexArray> ImportStaticMesh(const std::filesystem::path& filepath);
	};
}