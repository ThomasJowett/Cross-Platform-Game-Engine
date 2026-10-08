#pragma once

#include "Core/core.h"
#include "Asset/Texture.h"

// Filtering and wrapping used when a pipeline samples textures, independent of any one texture
class Sampler
{
public:
	virtual ~Sampler() = default;

	Texture::FilterMethod GetFilterMethod() const { return m_FilterMethod; }
	Texture::WrapMethod GetWrapMethod() const { return m_WrapMethod; }

	// Only does anything on OpenGL, like Texture::Bind; other backends bind samplers through a Pipeline
	virtual void Bind(uint32_t unit) const {}

	// Shared sampler for these settings, created on first use
	static Ref<Sampler> Get(Texture::FilterMethod filterMethod, Texture::WrapMethod wrapMethod);
	static void ClearCache();

protected:
	Sampler(Texture::FilterMethod filterMethod, Texture::WrapMethod wrapMethod)
		:m_FilterMethod(filterMethod), m_WrapMethod(wrapMethod) {}

	Texture::FilterMethod m_FilterMethod;
	Texture::WrapMethod m_WrapMethod;
};
