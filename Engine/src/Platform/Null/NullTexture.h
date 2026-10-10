#pragma once

#include "Asset/Texture.h"

// Texture for headless runs: reads the image size and channels but creates no GPU resource
class NullTexture2D : public Texture2D
{
public:
	NullTexture2D(uint32_t width, uint32_t height, Format format);
	NullTexture2D(const std::filesystem::path& filepath);
	NullTexture2D(const std::filesystem::path& filepath, const std::vector<uint8_t>& imageData);

	virtual uint32_t GetWidth() const override { return m_Width; }
	virtual uint32_t GetHeight() const override { return m_Height; }
	virtual uint32_t GetChannels() const override { return m_Channels; }

	virtual void SetData(const void* data) override {}

	virtual void Bind(uint32_t slot) const override {}

	virtual void* GetRendererID() const override { return nullptr; }

	virtual bool Reload() override;

	virtual bool operator==(const Texture& other) const override { return this == &other; }
private:
	bool ReadInfoFromFile();
	void SetPlaceholderSize();

	uint32_t m_Width = 0;
	uint32_t m_Height = 0;
	uint32_t m_Channels = 4;
};
