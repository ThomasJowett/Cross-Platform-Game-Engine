#pragma once

#include "Renderer/Sampler.h"

class OpenGLSampler : public Sampler
{
public:
	OpenGLSampler(Texture::FilterMethod filterMethod, Texture::WrapMethod wrapMethod);
	virtual ~OpenGLSampler();

	// Overrides the filter and wrap state of whatever texture is bound to this unit
	virtual void Bind(uint32_t unit) const override;

private:
	uint32_t m_RendererID = 0;
};
