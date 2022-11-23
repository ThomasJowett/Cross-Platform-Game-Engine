#pragma once

#include "Core/Asset.h"
#include "cereal/access.hpp"
#include "Texture.h"

class Font : public Asset
{
public:
	Font();
	Font(const std::filesystem::path& filepath);

	// Inherited via Asset
	virtual bool Load(const std::filesystem::path& filepath) override;
	const std::filesystem::path& GetFilepath() const override { return m_Filepath; }
private:
};