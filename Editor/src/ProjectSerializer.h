#pragma once

#include <filesystem>

#include "ProjectData.h"

// Reads/writes the .proj file as XML
namespace ProjectSerializer
{
	bool Serialize(const ProjectData& data, const std::filesystem::path& filepath);
	bool Deserialize(ProjectData& data, const std::filesystem::path& filepath);
}
