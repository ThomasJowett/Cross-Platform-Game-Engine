#include "WebGPURendererAPI.h"
#include "Core/Application.h"
#include "Logging/Instrumentor.h"
#include "WebGPUFrameBuffer.h"
#include <cstdint>
#include <webgpu/webgpu.hpp>

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
	PROFILE_FUNCTION();

	wgpu::RenderPassDescriptor renderPassDesc = {};

	std::vector<wgpu::RenderPassColorAttachment> colourAttachments;
	wgpu::RenderPassDepthStencilAttachment depthAttachment = {};

	WebGPUFrameBuffer* currentFrameBuffer = WebGPUFrameBuffer::GetCurrent();
	bool hasDepth = false;

	if (currentFrameBuffer)
	{
		const auto& colourViews = currentFrameBuffer->GetColourViews();
		if (colourViews.empty())
			return;

		for (const auto& view : colourViews)
		{
			wgpu::RenderPassColorAttachment attachment = {};
			attachment.view = view;
			attachment.loadOp = wgpu::LoadOp::Clear;
			attachment.storeOp = wgpu::StoreOp::Store;
			attachment.clearValue = wgpu::Color{ m_ClearColour.r, m_ClearColour.g, m_ClearColour.b, m_ClearColour.a };
#ifdef __EMSCRIPTEN__
			attachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;
#endif
			colourAttachments.push_back(attachment);
		}

		wgpu::TextureView depthView = currentFrameBuffer->GetDepthView();
		if (depthView)
		{
			hasDepth = true;
			depthAttachment.view = depthView;
			depthAttachment.depthLoadOp = wgpu::LoadOp::Clear;
			depthAttachment.depthStoreOp = wgpu::StoreOp::Store;
			depthAttachment.depthClearValue = 1.0f;

			depthAttachment.stencilLoadOp = wgpu::LoadOp::Clear;
			depthAttachment.stencilStoreOp = wgpu::StoreOp::Discard;
			depthAttachment.stencilClearValue = 0;
		}
	}
	else
	{
		// Swapchain
		m_CurrentTargetWidth = m_WebGPUContext->GetSurfaceConfig().width;
		m_CurrentTargetHeight = m_WebGPUContext->GetSurfaceConfig().height;

		wgpu::TextureView targetView = m_WebGPUContext->GetCurrentTextureView();
		if (targetView)
		{
			wgpu::RenderPassColorAttachment attachment = {};
			attachment.view = targetView;
			attachment.loadOp = wgpu::LoadOp::Clear;
			attachment.storeOp = wgpu::StoreOp::Store;
			attachment.clearValue = wgpu::Color{ m_ClearColour.r, m_ClearColour.g, m_ClearColour.b, m_ClearColour.a };
#ifdef __EMSCRIPTEN__
			attachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;
#endif
			colourAttachments.push_back(attachment);
		}
	}

	if (colourAttachments.empty() && !hasDepth)
		return;

	renderPassDesc.colorAttachmentCount = static_cast<uint32_t>(colourAttachments.size());
	renderPassDesc.colorAttachments = colourAttachments.data();
	if (hasDepth)
	{
		renderPassDesc.depthStencilAttachment = &depthAttachment;
	}
	else
	{
		renderPassDesc.depthStencilAttachment = nullptr;
	}

	if (!m_WebGPUContext)
		return;

	auto device = m_WebGPUContext->GetWebGPUDevice();
	if (!device)
		return;

	wgpu::CommandEncoderDescriptor encoderDesc = {};
	encoderDesc.label = "Render Command encoder";

	m_CommandEncoder = device.createCommandEncoder(encoderDesc);
	if (!m_CommandEncoder)
		return;

	m_RenderPass = m_CommandEncoder.beginRenderPass(renderPassDesc);
}

void WebGPURendererAPI::EndRenderPass()
{
	PROFILE_FUNCTION();
	if (m_RenderPass)
	{
		m_RenderPass.end();
		m_RenderPass = nullptr;
	}

	if (m_CommandEncoder)
	{
		wgpu::CommandBufferDescriptor cmdBufferDescriptor = {};
		cmdBufferDescriptor.label = "Renderer Command Buffer";
		wgpu::CommandBuffer command = m_CommandEncoder.finish(cmdBufferDescriptor);
		m_WebGPUContext->GetQueue().submit(1, &command);
		m_CommandEncoder = nullptr;
	}
}

void WebGPURendererAPI::DrawIndexed(uint32_t indexCount, uint32_t indexStart, uint32_t vertexOffset, bool backFaceCull, DrawMode drawMode)
{
	PROFILE_FUNCTION();
	m_RenderPass.drawIndexed(indexCount, 1, indexStart, vertexOffset, 0);
}

void WebGPURendererAPI::DrawLines(uint32_t vertexCount)
{
	PROFILE_FUNCTION();
	if (!m_RenderPass)
	{
		ENGINE_ERROR("WebGPURendererAPI: DrawLines called with null render pass!");
		return;
	}
	ENGINE_TRACE("WebGPURendererAPI: DrawLines count={0}", vertexCount);
	if (m_CurrentPipeline)
	{
		auto webGPUPipeline = std::dynamic_pointer_cast<WebGPUPipeline>(m_CurrentPipeline);
		if (webGPUPipeline)
			webGPUPipeline->CommitBindGroups();
	}
	m_RenderPass.draw(vertexCount, 1, 0, 0);
}

wgpu::RenderPassEncoder WebGPURendererAPI::GetRenderPass() { return m_RenderPass; }
