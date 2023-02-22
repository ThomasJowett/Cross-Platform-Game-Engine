#pragma once

#include "Renderer/Pipeline.h"

class DirectX11Pipeline : public Pipeline
{
public:
	DirectX11Pipeline(const Spec& spec);
	~DirectX11Pipeline();

	// Inherited via Pipeline
	virtual void Invalidate() override;
	virtual void SetUniformBuffer(Ref<UniformBuffer> uniformBuffer, uint32_t binding, uint32_t set) override;
	virtual void Bind() override;
};
