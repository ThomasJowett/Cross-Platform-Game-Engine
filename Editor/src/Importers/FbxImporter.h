#pragma once

#include "ofbx.h"

#include "Renderer/VertexArray.h"


class FbxImporter
{
public:
	static void ImportAssests(const std::filesystem::path& filepath, const std::filesystem::path& destination);
};