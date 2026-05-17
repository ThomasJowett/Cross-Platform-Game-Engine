#include "WebGPURendererAPI.h"
#include "Core/Application.h"
#include "Logging/Instrumentor.h"

bool WebGPURendererAPI::Init()
{
	PROFILE_FUNCTION();


	Ref<GraphicsContext> context = Application::GetWindow()->GetContext();

	m_WebGPUContext = std::dynamic_pointer_cast<WebGPUContext>(context);

	return true;
}

void WebGPURendererAPI::SetClearColour(const Colour& colour) { m_ClearColour = colour; }

void WebGPURendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
	if (m_RenderPass)
	{
		m_RenderPass.setViewport(static_cast<float>(x), static_cast<float>(y), static_cast<float>(width), static_cast<float>(height), 0.0f, 1.0f);
	}
}

void WebGPURendererAPI::Clear() {}

void WebGPURendererAPI::ClearColour() {}

void WebGPURendererAPI::ClearDepth() {}

void WebGPURendererAPI::StartRenderPass()
{
	// TODO: Implement this function
	// TODO: Pass the target view to this function and get the command encoder
	/*
	wgpu::RenderPassDescriptor renderPassDesc = {};

	auto device = m_WebGPUContext->GetWebGPUDevice();

	wgpu::CommandEncoderDescriptor encoderDesc = {};
	encoderDesc.label = "Command encoder";

	wgpu::CommandEncoder encoder = device.createCommandEncoder(encoderDesc);

	//TODO: render pass descriptor

	m_RenderPass = encoder.beginRenderPass(renderPassDesc);
	*/
}

void WebGPURendererAPI::EndRenderPass()
{
	if (m_RenderPass)
	{
		m_RenderPass.end();
		m_RenderPass.release();
		m_RenderPass = nullptr;
	}
}

void WebGPURendererAPI::DrawIndexed(uint32_t indexCount, uint32_t indexStart, uint32_t vertexOffset, bool backFaceCull, DrawMode drawMode)
{
	m_RenderPass.drawIndexed(indexCount, 1, indexStart, vertexOffset, 0);
}

void WebGPURendererAPI::DrawLines(uint32_t vertexCount) {}

wgpu::RenderPassEncoder WebGPURendererAPI::GetRenderPass() { return m_RenderPass; }
