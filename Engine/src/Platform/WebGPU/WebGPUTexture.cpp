#include "stdafx.h"
#include "WebGPUTexture.h"
#include "Core/Application.h"
#include "Logging/Instrumentor.h"

#include <stb/stb_image.h>
#include <filesystem>

void WebGPUTexture2D::CreateSampler()
{
	wgpu::SamplerDescriptor samplerDesc = {};
	switch (m_FilterMethod)
	{
	case Texture::FilterMethod::Linear:
		samplerDesc.minFilter = wgpu::FilterMode::Linear;
		samplerDesc.magFilter = wgpu::FilterMode::Linear;
		samplerDesc.mipmapFilter = wgpu::MipmapFilterMode::Linear;
		break;
	case Texture::FilterMethod::Nearest:
		samplerDesc.minFilter = wgpu::FilterMode::Nearest;
		samplerDesc.magFilter = wgpu::FilterMode::Nearest;
		samplerDesc.mipmapFilter = wgpu::MipmapFilterMode::Nearest;
		break;
	default:
		break;
	}

	switch (m_WrapMethod)
	{
	case Texture::WrapMethod::Clamp:
		samplerDesc.addressModeU = wgpu::AddressMode::ClampToEdge;
		samplerDesc.addressModeV = wgpu::AddressMode::ClampToEdge;
		samplerDesc.addressModeW = wgpu::AddressMode::ClampToEdge;
		break;
	case Texture::WrapMethod::Mirror:
		samplerDesc.addressModeU = wgpu::AddressMode::MirrorRepeat;
		samplerDesc.addressModeV = wgpu::AddressMode::MirrorRepeat;
		samplerDesc.addressModeW = wgpu::AddressMode::MirrorRepeat;
		break;
	case Texture::WrapMethod::Repeat:
		samplerDesc.addressModeU = wgpu::AddressMode::Repeat;
		samplerDesc.addressModeV = wgpu::AddressMode::Repeat;
		samplerDesc.addressModeW = wgpu::AddressMode::Repeat;
		break;
	default:
		break;
	}

	auto device = m_WebGPUContext->GetWebGPUDevice();

	m_Sampler = device.createSampler(samplerDesc);
}

WebGPUTexture2D::WebGPUTexture2D(uint32_t width, uint32_t height, Format format, uint32_t samples, const void* pixels)
	:m_Width(width), m_Height(height)
{
	PROFILE_FUNCTION();

	if (!pixels)
		m_Filepath = "NO DATA";

	m_TextureFormat = wgpu::TextureFormat::RGBA8Unorm;

	switch (format)
	{
	case Texture::Format::RED:      m_TextureFormat = wgpu::TextureFormat::R8Unorm;      break;
	case Texture::Format::RED8UI:   m_TextureFormat = wgpu::TextureFormat::R8Uint;       break;
	case Texture::Format::RED16UI:  m_TextureFormat = wgpu::TextureFormat::R16Uint;      break;
	case Texture::Format::RED32UI:  m_TextureFormat = wgpu::TextureFormat::R32Uint;      break;
	case Texture::Format::RED32F:   m_TextureFormat = wgpu::TextureFormat::R32Float;     break;
	case Texture::Format::RG8:      m_TextureFormat = wgpu::TextureFormat::RG8Unorm;     break;
	case Texture::Format::RG16F:    m_TextureFormat = wgpu::TextureFormat::RG16Float;    break;
	case Texture::Format::RG32F:    m_TextureFormat = wgpu::TextureFormat::RG32Float;    break;
	case Texture::Format::RGB:      m_TextureFormat = wgpu::TextureFormat::RGBA8Unorm;   break;//TODO: check this
	case Texture::Format::RGBA:     m_TextureFormat = wgpu::TextureFormat::RGBA8Unorm;   break;
	case Texture::Format::RGBA16F:  m_TextureFormat = wgpu::TextureFormat::RGBA16Float;  break;
	case Texture::Format::RGBA32F:  m_TextureFormat = wgpu::TextureFormat::RGBA16Float;  break;
	default: m_TextureFormat = wgpu::TextureFormat::RGBA8Unorm;	break;
	}

	m_TextureDesc.size.width = width;
	m_TextureDesc.size.height = height;
	m_TextureDesc.size.depthOrArrayLayers = 1;
	m_TextureDesc.mipLevelCount = 1;
	m_TextureDesc.sampleCount = samples;
	m_TextureDesc.dimension = wgpu::TextureDimension::_2D;
	m_TextureDesc.format = m_TextureFormat;
	m_TextureDesc.usage = wgpu::TextureUsage::TextureBinding | wgpu::TextureUsage::CopyDst | wgpu::TextureUsage::RenderAttachment;

	Ref<GraphicsContext> context = Application::GetWindow()->GetContext();

	m_WebGPUContext = std::dynamic_pointer_cast<WebGPUContext>(context);

	auto device = m_WebGPUContext->GetWebGPUDevice();

	m_Texture = device.createTexture(m_TextureDesc);
	if (!m_Texture) {
		ENGINE_ERROR("Failed to create WebGPU Texture");
	}

	m_TextureViewDesc.format = m_TextureFormat;
	m_TextureViewDesc.dimension = wgpu::TextureViewDimension::_2D;
	m_TextureViewDesc.baseMipLevel = 0;
	m_TextureViewDesc.mipLevelCount = 1;
	m_TextureViewDesc.baseArrayLayer = 0;
	m_TextureViewDesc.arrayLayerCount = 1;
	m_TextureView = m_Texture.createView(m_TextureViewDesc);

	if (pixels)
		SetData(pixels);

	CreateSampler();
}

WebGPUTexture2D::WebGPUTexture2D(const std::filesystem::path& path)
	:m_Height(0), m_Width(0)
{
	PROFILE_FUNCTION();

	m_Filepath = path;

	bool isValid = std::filesystem::exists(path);

	CORE_ASSERT(isValid, "Image does not exist! " + path.string());

	Ref<GraphicsContext> context = Application::GetWindow()->GetContext();

	m_WebGPUContext = std::dynamic_pointer_cast<WebGPUContext>(context);

	if (isValid) {
		isValid = LoadTextureFromFile();
	}
	else {
		NullTexture();
	}

	m_TextureViewDesc.format = m_TextureFormat;
	m_TextureViewDesc.dimension = wgpu::TextureViewDimension::_2D;
	m_TextureViewDesc.baseMipLevel = 0;
	m_TextureViewDesc.mipLevelCount = 1;
	m_TextureViewDesc.baseArrayLayer = 0;
	m_TextureViewDesc.arrayLayerCount = 1;
	m_TextureView = m_Texture.createView(m_TextureViewDesc);
}

WebGPUTexture2D::~WebGPUTexture2D()
{
	PROFILE_FUNCTION();
	if (Application::Get().IsRunning()) {
		m_Texture.destroy();
		m_Texture.release();
		m_Sampler.release();
		m_TextureView.release();
	}
}

void WebGPUTexture2D::SetData(const void* data)
{
	PROFILE_FUNCTION();

	m_Filepath = "";

	uint32_t bytesPerPixel = 0;

	switch (m_TextureFormat) {
	case wgpu::TextureFormat::R8Unorm:
	case wgpu::TextureFormat::R8Uint:
		bytesPerPixel = 1;
		break;
	case wgpu::TextureFormat::RG8Unorm:
	case wgpu::TextureFormat::R16Uint:
		bytesPerPixel = 2;
		break;
	case wgpu::TextureFormat::R32Uint:
	case wgpu::TextureFormat::R32Float:
	case wgpu::TextureFormat::RG16Float:
	case wgpu::TextureFormat::RGBA8Unorm:
		bytesPerPixel = 4;
		break;
	case wgpu::TextureFormat::RG32Float:
	case wgpu::TextureFormat::RGBA16Float:
		bytesPerPixel = 8;
		break;
	case wgpu::TextureFormat::RGBA32Float:
		bytesPerPixel = 16;
		break;
	default:
		ENGINE_ERROR("Unsupported texture format");
	}

	uint32_t dataSize = m_Width * m_Height * bytesPerPixel;

	wgpu::BufferDescriptor bufferDesc = {};
	bufferDesc.usage = wgpu::BufferUsage::CopySrc;
	bufferDesc.size = dataSize;
	bufferDesc.mappedAtCreation = true;

	auto device = m_WebGPUContext->GetWebGPUDevice();

	wgpu::Buffer stagingBuffer = device.createBuffer(bufferDesc);
	if (!stagingBuffer) {
		ENGINE_ERROR("Failed to create staging buffer for texture upload");
		return;
	}

	void* mappedData = stagingBuffer.getMappedRange(0, dataSize);
	std::memcpy(mappedData, data, dataSize);
	stagingBuffer.unmap();

	wgpu::ImageCopyBuffer imageCopyBuffer = {};
	imageCopyBuffer.buffer = stagingBuffer;
	imageCopyBuffer.layout.offset = 0;
	imageCopyBuffer.layout.bytesPerRow = m_Width * bytesPerPixel;
	imageCopyBuffer.layout.rowsPerImage = m_Height;

	wgpu::ImageCopyTexture imageCopyTexture = {};
	imageCopyTexture.texture = m_Texture;
	imageCopyTexture.mipLevel = 0;
	imageCopyTexture.origin = { 0,0,0 };
	imageCopyTexture.aspect = wgpu::TextureAspect::All;

	wgpu::Extent3D textureExtent = { m_Width, m_Height, 1 };

	auto queue = m_WebGPUContext->GetQueue();
	queue.writeTexture(imageCopyTexture, data, dataSize, imageCopyBuffer.layout, textureExtent);

	stagingBuffer.destroy();
	stagingBuffer.release();
}

void WebGPUTexture2D::Bind(uint32_t slot) const
{
	PROFILE_FUNCTION();
	// TODO: set Bind group
}

std::string WebGPUTexture2D::GetName() const
{
	return m_Filepath.filename().string();
}

void* WebGPUTexture2D::GetRendererID() const
{
	return m_BindGroup;
}

void WebGPUTexture2D::Reload()
{
	if (!m_Filepath.empty() || m_Filepath != "NO DATA")
	{
		m_Texture.destroy();
		m_Texture.release();
		m_Sampler.release();
		m_TextureView.release();
		LoadTextureFromFile();
	}
}

bool WebGPUTexture2D::operator==(const Texture& other) const
{
	return m_BindGroup == ((WebGPUTexture2D&)other).GetRendererID();
}

void WebGPUTexture2D::SetFilterMethod(FilterMethod filterMethod)
{
	m_FilterMethod = filterMethod;
	CreateSampler();
}

void WebGPUTexture2D::SetWrapMethod(WrapMethod wrapMethod)
{
	m_WrapMethod = wrapMethod;
	CreateSampler();
}

void WebGPUTexture2D::NullTexture()
{
	m_Filepath = "NULL";
	m_Width = m_Height = 4;

	m_TextureFormat = wgpu::TextureFormat::RGBA8Unorm;

	uint32_t textureData[4][4];

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			textureData[i][j] = ((i + j) % 2) ? 0xffff00ff : 0xff000000;
		}
	}

	m_TextureDesc.size.width = m_Width;
	m_TextureDesc.size.height = m_Height;
	m_TextureDesc.size.depthOrArrayLayers = 1;
	m_TextureDesc.mipLevelCount = 1;
	m_TextureDesc.sampleCount = 1;
	m_TextureDesc.dimension = wgpu::TextureDimension::_2D;
	m_TextureDesc.format = m_TextureFormat;
	m_TextureDesc.usage = wgpu::TextureUsage::TextureBinding | wgpu::TextureUsage::CopyDst;

	auto device = m_WebGPUContext->GetWebGPUDevice();
	auto queue = m_WebGPUContext->GetQueue();

	m_Texture = device.createTexture(m_TextureDesc);

	wgpu::ImageCopyTexture imageCopyTexture{};
	imageCopyTexture.texture = m_Texture;
	imageCopyTexture.mipLevel = 0;
	imageCopyTexture.origin = { 0,0,0 };

	wgpu::TextureDataLayout dataLayout{};
	dataLayout.offset = 0;
	dataLayout.bytesPerRow = m_Width * sizeof(uint32_t);
	dataLayout.rowsPerImage = m_Height;

	wgpu::Extent3D textureSize{};
	textureSize.width = m_Width;
	textureSize.height = m_Height;
	textureSize.depthOrArrayLayers = 1;

	queue.writeTexture(imageCopyTexture, textureData, sizeof(textureData), dataLayout, textureSize);

	CreateSampler();
}

bool WebGPUTexture2D::LoadTextureFromFile()
{
	PROFILE_FUNCTION();

	int width, height, channels;
	stbi_set_flip_vertically_on_load(1);
	stbi_uc* data = nullptr;
	{
		PROFILE_SCOPE("stbi Load Image WebGPUTexture2D(const std::string&)");
		data = stbi_load(m_Filepath.string().c_str(), &width, &height, &channels, 0);
	}

	CORE_ASSERT(data, "Failed to load image! " + m_Filepath.string());

	if (!data)
	{
		ENGINE_ERROR("stb image failure: {0}", stbi_failure_reason());
		return false;
	}

	m_Width = (uint32_t)width;
	m_Height = (uint32_t)height;

	if (channels == 4)
	{
		m_TextureFormat = wgpu::TextureFormat::RGBA8Unorm;
	}
	else if (channels == 3)
	{
		m_TextureFormat = wgpu::TextureFormat::RGBA8Unorm;

		//WebGPU does not support 3 channel textures so we must convert into 4

		ENGINE_WARN("Converting RGB to RGBA for compatibility with WebGPU");

		stbi_uc* rgbaData = new stbi_uc[m_Width * m_Height * 4];
		for (uint32_t i = 0; i < m_Width * m_Height; ++i) {
			rgbaData[i * 4 + 0] = data[i * 3 + 0];
			rgbaData[i * 4 + 1] = data[i * 3 + 1];
			rgbaData[i * 4 + 2] = data[i * 3 + 2];
			rgbaData[i * 4 + 3] = 255;
		}

		channels = 4;

		stbi_image_free(data);
		data = rgbaData;
	}
	else if (channels == 1)
	{
		m_TextureFormat = wgpu::TextureFormat::R8Unorm;
	}
	else {
		ENGINE_ERROR("Texture format not supported");
		return false;
	}

	m_TextureDesc.size.width = m_Width;
	m_TextureDesc.size.height = m_Height;
	m_TextureDesc.size.depthOrArrayLayers = 1;
	m_TextureDesc.mipLevelCount = 1; //TODO: implement mipmap generation
	m_TextureDesc.sampleCount = 1;
	m_TextureDesc.dimension = wgpu::TextureDimension::_2D;
	m_TextureDesc.format = m_TextureFormat;
	m_TextureDesc.usage = wgpu::TextureUsage::TextureBinding | wgpu::TextureUsage::CopyDst;

	auto device = m_WebGPUContext->GetWebGPUDevice();
	auto queue = m_WebGPUContext->GetQueue();

	m_Texture = device.createTexture(m_TextureDesc);
	
	wgpu::ImageCopyTexture imageCopyTexture{};
	imageCopyTexture.texture = m_Texture;
	imageCopyTexture.mipLevel = 0;
	imageCopyTexture.origin = { 0,0,0 };

	wgpu::TextureDataLayout dataLayout{};
	dataLayout.offset = 0;
	dataLayout.bytesPerRow = m_Width * channels;
	dataLayout.rowsPerImage = m_Height;

	wgpu::Extent3D textureSize{};
	textureSize.width = m_Width;
	textureSize.height = m_Height;
	textureSize.depthOrArrayLayers = 1;

	queue.writeTexture(imageCopyTexture, data, m_Width * m_Height * channels, dataLayout, textureSize);

	CreateSampler();

	stbi_image_free(data);

	return true;
}
