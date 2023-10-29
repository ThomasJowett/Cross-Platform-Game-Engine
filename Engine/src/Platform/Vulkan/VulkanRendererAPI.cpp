#include "stdafx.h"
#include "VulkanRendererAPI.h"

#include "Vulkan.h"

VulkanRendererAPI::~VulkanRendererAPI()
{
}

bool VulkanRendererAPI::Init()
{
	return false;
}

void VulkanRendererAPI::SetClearColour(const Colour& colour)
{

}

void VulkanRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
}

void VulkanRendererAPI::Clear()
{
}

void VulkanRendererAPI::ClearColour()
{
}

void VulkanRendererAPI::ClearDepth()
{
}

void VulkanRendererAPI::DrawIndexed(uint32_t indexCount, uint32_t startIndex, uint32_t vertexOffset, bool backFaceCull, DrawMode drawMode)
{
}

void VulkanRendererAPI::DrawLines(uint32_t vertexCount)
{
}