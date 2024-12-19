#include "stdafx.h"
#include "WebGPUPipeline.h"

#include <glad/glad.h>

WebGPUPipeline::WebGPUPipeline(const Spec& spec)
{
	m_TransparencyEnabled = spec.transparencyEnabled;
	m_BackfaceCull = spec.backFaceCulling;
	m_Specification = spec;
}

WebGPUPipeline::~WebGPUPipeline()
{
}

void WebGPUPipeline::Invalidate()
{
}

void WebGPUPipeline::SetUniformBuffer(Ref<UniformBuffer> uniformBuffer, uint32_t binding, uint32_t set)
{
}

void WebGPUPipeline::Bind()
{
}