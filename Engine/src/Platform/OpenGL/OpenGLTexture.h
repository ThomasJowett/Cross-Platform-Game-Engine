#pragma once

#include "Renderer/Texture.h"

class OpenGLTexture2D : public Texture2D
{
public:
	OpenGLTexture2D(std::string path);
	virtual ~OpenGLTexture2D();

	virtual uint32_t GetWidth() const override { return m_Width; }
	virtual uint32_t GetHeight() const override { return m_Height; }

	virtual void Bind(uint32_t slot) const override;
private:
	std::string m_Path;
	uint32_t m_Width, m_Height;

	uint32_t m_RendererID;
};
