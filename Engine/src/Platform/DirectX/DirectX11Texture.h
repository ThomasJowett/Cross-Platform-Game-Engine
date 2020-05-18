#pragma once

#include "Renderer/Texture.h"

class DirectX11Texture2D :public Texture2D
{
public:
	DirectX11Texture2D(uint32_t width, uint32_t height);
	DirectX11Texture2D(const std::string& path);
	virtual ~DirectX11Texture2D();

	virtual uint32_t GetWidth() const override { return m_Width; }
	virtual uint32_t GetHeight() const override { return m_Height; }

	virtual void SetData(void* data, uint32_t size) override;

	virtual void Bind(uint32_t slot) const override;

	virtual std::string GetName() const override;

	virtual uint32_t GetRendererID() const override;


	virtual bool operator==(const Texture& other) const;
private:
	// sets the texture to be the null texture
	void NullTexture();

	bool LoadTextureFromFile();
	std::string m_Path;
	uint32_t m_Width, m_Height;

	ID3D11ShaderResourceView* m_ShaderResourceView;
};