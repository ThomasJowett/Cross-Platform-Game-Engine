#include "WebGPUFrameBuffer.h"
#include "WebGPUContext.h"
#include "Logging/Instrumentor.h"

#include <webgpu/webgpu.hpp>

WebGPUFrameBuffer::WebGPUFrameBuffer(const FrameBufferSpecification& specification)
	:m_Specification(specification), m_DepthAttachment(0)
{
	PROFILE_FUNCTION();
	auto context = std::dynamic_pointer_cast<WebGPUContext>(Application::GetWindow()->GetContext());
	m_Device = context->GetWebGPUDevice();
	Generate();
}

WebGPUFrameBuffer::~WebGPUFrameBuffer()
{
	Destroy();
}

void WebGPUFrameBuffer::Bind()
{
}

void WebGPUFrameBuffer::UnBind()
{
}

void WebGPUFrameBuffer::Generate()
{
	PROFILE_FUNCTION();

	Destroy();

	if (m_Specification.height == 0 || m_Specification.width == 0)
	{
		return;
	}

	bool multisample = m_Specification.samples > 1;

	// Attachments
	for (auto& spec : m_Specification.attachments.attachments)
	{
		if (FrameBuffer::IsDepthFormat(spec.textureFormat))
		{
			m_DepthAttachment = CreateRef<WebGPUTexture2D>(m_Specification.width, m_Specification.height,
				FrameBufferFormatToTextureFormat(spec.textureFormat),
				m_Specification.samples, nullptr);
			m_DepthView = m_DepthAttachment->GetTextureView();
		}
		else
		{
			auto colorTex = CreateRef<WebGPUTexture2D>(
				m_Specification.width, m_Specification.height,
				FrameBufferFormatToTextureFormat(spec.textureFormat),
				m_Specification.samples, nullptr);
			m_ColourViews.push_back(colorTex->GetTextureView());
			m_ColourAttachments.push_back(colorTex);
		}
	}
}

void WebGPUFrameBuffer::Destroy()
{
	PROFILE_FUNCTION();
	m_ColourAttachments.clear();
	m_DepthAttachment.reset();

	m_ColourViews.clear();
	m_DepthView = nullptr;
}

void WebGPUFrameBuffer::Resize(uint32_t width, uint32_t height)
{
	PROFILE_FUNCTION();
	m_Specification.width = width;
	m_Specification.height = height;
	Destroy();
	Generate();
}

int WebGPUFrameBuffer::ReadPixel(uint32_t attachmentIndex, int x, int y)
{
	PROFILE_FUNCTION();
	CORE_ASSERT(attachmentIndex < m_ColourAttachments.size(), "Trying to access attachment that does not exist!");
	int pixelData = m_ColourAttachments[attachmentIndex]->ReadPixel(x, y);
	return pixelData;
}

Ref<Texture> WebGPUFrameBuffer::GetColourAttachment(size_t index)
{
	CORE_ASSERT(index < m_ColourAttachments.size(), "Index out of range");
	return m_ColourAttachments[index];
}

Ref<Texture> WebGPUFrameBuffer::GetDepthAttachment()
{
	return m_DepthAttachment;
}

void WebGPUFrameBuffer::BlitDepthTo(Ref<FrameBuffer> target)
{
	PROFILE_FUNCTION();
	// Not natively supported in WebGPU, so we use a shader to copy the depth buffer
}

void WebGPUFrameBuffer::BlitColourTo(Ref<FrameBuffer> target, uint32_t srcAttachmentIndex, uint32_t dstAttachmentIndex)
{
	PROFILE_FUNCTION();
	// Not natively supported in WebGPU, so we use a shader to copy the colour buffer
}

void WebGPUFrameBuffer::ClearAttachment(size_t index, int value)
{
	PROFILE_FUNCTION();
}