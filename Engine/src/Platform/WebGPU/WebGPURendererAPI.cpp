#include "WebGPURendererAPI.h"
#include "Core/Application.h"
#include "Logging/Instrumentor.h"
#include "WebGPUFrameBuffer.h"
#include "WebGPUPipeline.h"
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
		float v_width = static_cast<float>(width);
		float v_height = static_cast<float>(height);

		ENGINE_TRACE("WebGPURendererAPI: SetViewport {0}, {1}, {2}, {3} (Target: {4}x{5})", x, y, width, height, m_CurrentTargetWidth, m_CurrentTargetHeight);

		if (m_CurrentTargetWidth > 0 && width > m_CurrentTargetWidth)
		{
			ENGINE_TRACE("WebGPURendererAPI: Clamping viewport width {0} -> {1}", width, m_CurrentTargetWidth);
			v_width = static_cast<float>(m_CurrentTargetWidth);
		}
		if (m_CurrentTargetHeight > 0 && height > m_CurrentTargetHeight)
		{
			ENGINE_TRACE("WebGPURendererAPI: Clamping viewport height {0} -> {1}", height, m_CurrentTargetHeight);
			v_height = static_cast<float>(m_CurrentTargetHeight);
		}

		m_RenderPass.setViewport(static_cast<float>(x), static_cast<float>(y), v_width, v_height, 0.0f, 1.0f);
	}
}

void WebGPURendererAPI::Clear() {}

void WebGPURendererAPI::ClearColour() {}

void WebGPURendererAPI::ClearDepth() {}

void WebGPURendererAPI::StartRenderPass(bool clear)
{
	PROFILE_FUNCTION();
	ENGINE_TRACE("WebGPURendererAPI: StartRenderPass");

	wgpu::LoadOp loadOp = clear ? wgpu::LoadOp::Clear : wgpu::LoadOp::Load;

	wgpu::RenderPassDescriptor renderPassDesc = {};

	std::vector<wgpu::RenderPassColorAttachment> colourAttachments;
	wgpu::RenderPassDepthStencilAttachment depthAttachment = {};

	WebGPUFrameBuffer* currentFrameBuffer = WebGPUFrameBuffer::GetCurrent();
	bool hasDepth = false;
	bool targetingSwapchain = false;

	if (currentFrameBuffer)
	{
		m_CurrentTargetWidth = currentFrameBuffer->GetSpecification().width;
		m_CurrentTargetHeight = currentFrameBuffer->GetSpecification().height;

		const auto& colourViews = currentFrameBuffer->GetColourViews();
		const auto& specAttachments = currentFrameBuffer->GetSpecification().attachments.attachments;

		// Colour resolves as part of ending this pass, via resolveTarget - WebGPU has no native
		// resolve for integer formats (RED_INTEGER), so that one is left genuinely multisampled
		// here and resolved separately afterward (WebGPUFrameBuffer::ResolveTo).
		auto resolveTargetWebGPU = std::dynamic_pointer_cast<WebGPUFrameBuffer>(currentFrameBuffer->GetResolveTarget());
		bool shouldResolve = currentFrameBuffer->GetSpecification().samples > 1 && resolveTargetWebGPU;
		const auto& resolveViews = resolveTargetWebGPU ? resolveTargetWebGPU->GetColourViews() : colourViews;

		// colourViews only contains the non-depth attachments, in the same relative order they
		// appear in specAttachments - walk both together so each view can be matched to its format.
		size_t colourViewIndex = 0;
		for (const auto& spec : specAttachments)
		{
			if (FrameBuffer::IsDepthFormat(spec.textureFormat))
				continue;
			if (colourViewIndex >= colourViews.size())
				break;

			size_t thisColourIndex = colourViewIndex;
			wgpu::RenderPassColorAttachment attachment = {};
			attachment.view = colourViews[colourViewIndex++];
			attachment.loadOp = loadOp;
			attachment.storeOp = wgpu::StoreOp::Store;

			if (spec.textureFormat == FrameBufferTextureFormat::RED_INTEGER)
			{
				// Entity-id attachment: -1 means "no entity". Clearing it with the RGBA colour meant
				// for the visible attachment writes nonsense into this integer target (the colour's
				// R channel, rounded, e.g. 0 - which aliases a real entity index) rather than -1,
				// which made every pixel that was never drawn to resolve as entity 0.
				attachment.clearValue = wgpu::Color{ -1.0, 0.0, 0.0, 0.0 };
			}
			else
			{
				attachment.clearValue = wgpu::Color{ m_ClearColour.r, m_ClearColour.g, m_ClearColour.b, m_ClearColour.a };
				if (shouldResolve && thisColourIndex < resolveViews.size())
					attachment.resolveTarget = resolveViews[thisColourIndex];
			}
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
			depthAttachment.depthLoadOp = loadOp;
			depthAttachment.depthStoreOp = wgpu::StoreOp::Store;
			depthAttachment.depthClearValue = 1.0f;

			depthAttachment.stencilLoadOp = loadOp;
			depthAttachment.stencilStoreOp = wgpu::StoreOp::Discard;
			depthAttachment.stencilClearValue = 0;
		}
	}
	else
	{
		// Swapchain
		targetingSwapchain = true;
		m_CurrentTargetWidth = m_WebGPUContext->GetSurfaceConfig().width;
		m_CurrentTargetHeight = m_WebGPUContext->GetSurfaceConfig().height;

		wgpu::TextureView targetView = m_WebGPUContext->GetCurrentTextureView();
		if (targetView)
		{
			wgpu::RenderPassColorAttachment attachment = {};
			attachment.view = targetView;
			attachment.loadOp = loadOp;
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

	if (targetingSwapchain && m_RenderPass)
		m_WebGPUContext->MarkSwapchainRenderedThisFrame();
}

void WebGPURendererAPI::StartSingleAttachmentRenderPass(wgpu::TextureView colourView, uint32_t width, uint32_t height)
{
	PROFILE_FUNCTION();

	if (!m_WebGPUContext)
		return;

	auto device = m_WebGPUContext->GetWebGPUDevice();
	if (!device)
		return;

	m_CurrentTargetWidth = width;
	m_CurrentTargetHeight = height;

	wgpu::RenderPassColorAttachment attachment = {};
	attachment.view = colourView;
	attachment.loadOp = wgpu::LoadOp::Load;
	attachment.storeOp = wgpu::StoreOp::Store;
#ifdef __EMSCRIPTEN__
	attachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;
#endif

	wgpu::RenderPassDescriptor renderPassDesc = {};
	renderPassDesc.colorAttachmentCount = 1;
	renderPassDesc.colorAttachments = &attachment;
	renderPassDesc.depthStencilAttachment = nullptr;

	wgpu::CommandEncoderDescriptor encoderDesc = {};
	encoderDesc.label = "Entity-id resolve command encoder";

	m_CommandEncoder = device.createCommandEncoder(encoderDesc);
	if (!m_CommandEncoder)
		return;

	m_RenderPass = m_CommandEncoder.beginRenderPass(renderPassDesc);
}

void WebGPURendererAPI::StartDepthOnlyRenderPass(wgpu::TextureView depthView, uint32_t width, uint32_t height)
{
	PROFILE_FUNCTION();

	if (!m_WebGPUContext)
		return;

	auto device = m_WebGPUContext->GetWebGPUDevice();
	if (!device)
		return;

	m_CurrentTargetWidth = width;
	m_CurrentTargetHeight = height;

	wgpu::RenderPassDepthStencilAttachment depthAttachment = {};
	depthAttachment.view = depthView;
	depthAttachment.depthLoadOp = wgpu::LoadOp::Load;
	depthAttachment.depthStoreOp = wgpu::StoreOp::Store;
	depthAttachment.depthClearValue = 1.0f;
	depthAttachment.stencilLoadOp = wgpu::LoadOp::Load;
	depthAttachment.stencilStoreOp = wgpu::StoreOp::Store;
	depthAttachment.stencilClearValue = 0;

	wgpu::RenderPassDescriptor renderPassDesc = {};
	renderPassDesc.colorAttachmentCount = 0;
	renderPassDesc.colorAttachments = nullptr;
	renderPassDesc.depthStencilAttachment = &depthAttachment;

	wgpu::CommandEncoderDescriptor encoderDesc = {};
	encoderDesc.label = "Depth blit command encoder";

	m_CommandEncoder = device.createCommandEncoder(encoderDesc);
	if (!m_CommandEncoder)
		return;

	m_RenderPass = m_CommandEncoder.beginRenderPass(renderPassDesc);
}

void WebGPURendererAPI::EndRenderPass()
{
	PROFILE_FUNCTION();
	ENGINE_TRACE("WebGPURendererAPI: EndRenderPass");
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

void WebGPURendererAPI::DrawIndexed(uint32_t indexCount, uint32_t indexStart, uint32_t vertexOffset)
{
	PROFILE_FUNCTION();
	if (!m_RenderPass)
	{
		ENGINE_ERROR("WebGPURendererAPI: DrawIndexed called with null render pass!");
		return;
	}
	ENGINE_TRACE("WebGPURendererAPI: DrawIndexed count={0}", indexCount);
	if (m_CurrentPipeline)
	{
		auto webGPUPipeline = std::dynamic_pointer_cast<WebGPUPipeline>(m_CurrentPipeline);
		if (webGPUPipeline)
			webGPUPipeline->CommitBindGroups();
	}
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
