#pragma once

#include "Asset/Texture.h"

#include <d3d11.h>

class DirectX11Texture2D :public Texture2D
{
public:
	DirectX11Texture2D(uint32_t width, uint32_t height, uint32_t samples, Format format);
	DirectX11Texture2D(const std::filesystem::path& path);
	DirectX11Texture2D(const std::filesystem::path& path, const std::vector<uint8_t>& data);
	virtual ~DirectX11Texture2D();

	virtual uint32_t GetWidth() const override { return m_Width; }
	virtual uint32_t GetHeight() const override { return m_Height; }

	virtual void SetData(const void* data) override;

	virtual void Bind(uint32_t slot) const override;

	virtual uint32_t GetRendererID() const override;

	virtual bool Reload() override;

	virtual bool operator==(const Texture& other) const;
private:
	// sets the texture to be the null texture
	void NullTexture();

	bool LoadTextureFromFile();
	uint32_t m_Width, m_Height;

	ID3D11ShaderResourceView* m_ShaderResourceView;
};