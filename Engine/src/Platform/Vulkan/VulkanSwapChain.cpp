#include "stdafx.h"
#include "VulkanSwapChain.h"

#include "Core/Settings.h"
#include "Logging/Instrumentor.h"
#include <GLFW/glfw3.h>
//#include <limits>

void VulkanSwapChain::Init(VkInstance instance, Ref<VulkanDevice> device)
{
	m_Instance = instance;
	m_Device = device;
}

void VulkanSwapChain::InitSurface(GLFWwindow* windowHandle)
{
	glfwCreateWindowSurface(m_Instance, windowHandle, nullptr, &m_Surface);

	VkPhysicalDevice physicalDevice = m_Device->GetPyhsicalDevice()->GetVkPhysicalDevice();

	uint32_t formatCount;
	VK_CHECK_RESULT(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_Surface, &formatCount, NULL));
	ASSERT(formatCount > 0, "Could not find any surface formats");

	std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
	VK_CHECK_RESULT(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_Surface, &formatCount, surfaceFormats.data()));

	// If the surface format list only includes one entry with VK_FORMAT_UNDEFINED,
	// there is no preferered format, so we assume VK_FORMAT_B8G8R8A8_UNORM
	if ((formatCount == 1) && (surfaceFormats[0].format == VK_FORMAT_UNDEFINED))
	{
		m_ColourFormat = VK_FORMAT_B8G8R8A8_UNORM;
		m_ColourSpace = surfaceFormats[0].colorSpace;
	}
	else
	{
		// iterate over the list of available surface format and
		// check for the presence of VK_FORMAT_B8G8R8A8_UNORM
		bool found_B8G8R8A8_UNORM = false;
		for (auto&& surfaceFormat : surfaceFormats)
		{
			if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_UNORM)
			{
				m_ColourFormat = surfaceFormat.format;
				m_ColourSpace = surfaceFormat.colorSpace;
				found_B8G8R8A8_UNORM = true;
				break;
			}
		}

		// in case VK_FORMAT_B8G8R8A8_UNORM is not available
		// select the first available color format
		if (!found_B8G8R8A8_UNORM)
		{
			m_ColourFormat = surfaceFormats[0].format;
			m_ColourSpace = surfaceFormats[0].colorSpace;
		}
	}
}

void VulkanSwapChain::Create(int* width, int* height)
{
	bool vsync = Settings::GetBool("Display", "V-Sync");
	VkDevice device = m_Device->GetVkDevice();
	VkPhysicalDevice physicalDevice = m_Device->GetPyhsicalDevice()->GetVkPhysicalDevice();

	VkSwapchainKHR oldSwapChain = m_Swapchain;

	VkSurfaceCapabilitiesKHR surfaceCapabilites;

	VK_CHECK_RESULT(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, m_Surface, &surfaceCapabilites));

	uint32_t presentModeCount;
	VK_CHECK_RESULT(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_Surface, &presentModeCount, NULL));

	ASSERT(presentModeCount > 0, "There must be more than one present mode");
	std::vector<VkPresentModeKHR> presentModes(presentModeCount);
	VK_CHECK_RESULT(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_Surface, &presentModeCount, presentModes.data()));

	VkExtent2D swapChainExtent = {};
	if (surfaceCapabilites.currentExtent.width == std::numeric_limits<uint32_t>::max()) {
		swapChainExtent.width = *width;
		swapChainExtent.height = *height;
	}
	else {
		swapChainExtent = surfaceCapabilites.currentExtent;
		*width = surfaceCapabilites.currentExtent.width;
		*height = surfaceCapabilites.currentExtent.height;
	}

	// Select a present mode for the swapchain

	VkPresentModeKHR swapChainPresentMode = VK_PRESENT_MODE_FIFO_KHR;
	if (!vsync) {
		for (size_t i = 0; i < presentModeCount; i++)
		{
			if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				swapChainPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
				break;
			}
			if ((swapChainPresentMode != VK_PRESENT_MODE_MAILBOX_KHR) && (presentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR))
			{
				swapChainPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
			}
		}
	}

	// Determine the number of images
	uint32_t desiredNumberOfSwapChainImages = surfaceCapabilites.minImageCount + 1;
	if ((surfaceCapabilites.maxImageCount > 0) && (desiredNumberOfSwapChainImages > surfaceCapabilites.maxImageCount))
	{
		desiredNumberOfSwapChainImages = surfaceCapabilites.maxImageCount;
	}

	// Find the transformation of the surface
	VkSurfaceTransformFlagsKHR preTransform;
	if (surfaceCapabilites.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
	{
		preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	}
	else {
		preTransform = surfaceCapabilites.currentTransform;
	}

	// Find a supported composite alpha format (not all devices support alpha opaque)
	VkCompositeAlphaFlagBitsKHR compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	// Simply select the first composite alpha format available
	std::vector<VkCompositeAlphaFlagBitsKHR> compositeAlphaFlags = {
		VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
		VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
		VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
		VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
	};
	for (auto& compositeAlphaFlag : compositeAlphaFlags) {
		if (surfaceCapabilites.supportedCompositeAlpha & compositeAlphaFlag) {
			compositeAlpha = compositeAlphaFlag;
			break;
		};
	}

	VkSwapchainCreateInfoKHR swapchainCI = {};
	swapchainCI.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchainCI.pNext = NULL;
	swapchainCI.surface = m_Surface;
	swapchainCI.minImageCount = desiredNumberOfSwapChainImages;
	swapchainCI.imageFormat = m_ColourFormat;
	swapchainCI.imageColorSpace = m_ColourSpace;
	swapchainCI.imageExtent = { swapChainExtent.width, swapChainExtent.height };
	swapchainCI.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	swapchainCI.preTransform = (VkSurfaceTransformFlagBitsKHR)preTransform;
	swapchainCI.imageArrayLayers = 1;
	swapchainCI.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	swapchainCI.queueFamilyIndexCount = 0;
	swapchainCI.pQueueFamilyIndices = NULL;
	swapchainCI.presentMode = swapChainPresentMode;
	swapchainCI.oldSwapchain = oldSwapChain;
	// Setting clipped to VK_TRUE allows the implementation to discard rendering outside of the surface area
	swapchainCI.clipped = VK_TRUE;
	swapchainCI.compositeAlpha = compositeAlpha;

	// Set additional usage flag for blitting from the swapchain images if supported
	VkFormatProperties formatProps;
	vkGetPhysicalDeviceFormatProperties(physicalDevice, m_ColourFormat, &formatProps);
	if ((formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR) || (formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_SRC_BIT)) {
		swapchainCI.imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	}

	VK_CHECK_RESULT(vkCreateSwapchainKHR(device, &swapchainCI, nullptr, &m_Swapchain));

	if (oldSwapChain != VK_NULL_HANDLE)
	{
		for (SwapChainImage& image : m_Images)
			vkDestroyImageView(device, image.ImageView, nullptr);
		vkDestroySwapchainKHR(device, oldSwapChain, nullptr);
	}

	m_Images.clear();

	VK_CHECK_RESULT(vkGetSwapchainImagesKHR(device, m_Swapchain, &m_ImageCount, NULL));

	//Get the swap chain images
	m_Images.resize(m_ImageCount);
	m_VulkanImages.resize(m_ImageCount);
	VK_CHECK_RESULT(vkGetSwapchainImagesKHR(device, m_Swapchain, &m_ImageCount, m_VulkanImages.data()));

	m_Images.resize(m_ImageCount);
	for (uint32_t i = 0; i < m_ImageCount; i++)
	{
		VkImageViewCreateInfo colourAttachmentView = {};
		colourAttachmentView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		colourAttachmentView.pNext = NULL;
		colourAttachmentView.format= m_ColourFormat;
		colourAttachmentView.components = {
			VK_COMPONENT_SWIZZLE_R,
			VK_COMPONENT_SWIZZLE_G,
			VK_COMPONENT_SWIZZLE_B,
			VK_COMPONENT_SWIZZLE_A
		};
		colourAttachmentView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		colourAttachmentView.subresourceRange.baseMipLevel = 0;
		colourAttachmentView.subresourceRange.levelCount = 1;
		colourAttachmentView.subresourceRange.baseArrayLayer = 0;
		colourAttachmentView.subresourceRange.layerCount = 1;
		colourAttachmentView.viewType = VK_IMAGE_VIEW_TYPE_2D;
		colourAttachmentView.flags = 0;

		m_Images[i].Image = m_VulkanImages[i];

		colourAttachmentView.image = m_Images[i].Image;

		VK_CHECK_RESULT(vkCreateImageView(device, &colourAttachmentView, nullptr, &m_Images[i].ImageView));
	}

	if (!m_Semaphores.RenderComplete || !m_Semaphores.PresentComplete)
	{
		VkSemaphoreCreateInfo semaphoreCreateInfo{};
		semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		VK_CHECK_RESULT(vkCreateSemaphore(m_Device->GetVkDevice(), &semaphoreCreateInfo, nullptr, &m_Semaphores.RenderComplete));
		VK_CHECK_RESULT(vkCreateSemaphore(m_Device->GetVkDevice(), &semaphoreCreateInfo, nullptr, &m_Semaphores.PresentComplete));
	}

	if (m_WaitFences.size() != m_ImageCount)
	{
		VkFenceCreateInfo fenceCreateInfo{};
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		m_WaitFences.resize(m_ImageCount);
		for (auto& fence : m_WaitFences)
		{
			VK_CHECK_RESULT(vkCreateFence(m_Device->GetVkDevice(), &fenceCreateInfo, nullptr, &fence));
		}
	}

	VkPipelineStageFlags pipelineStageFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

	m_SubmitInfo = {};
	m_SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	m_SubmitInfo.pWaitDstStageMask = &pipelineStageFlags;
	m_SubmitInfo.waitSemaphoreCount = 1;
	m_SubmitInfo.pWaitSemaphores = &m_Semaphores.PresentComplete;
	m_SubmitInfo.signalSemaphoreCount = 1;
	m_SubmitInfo.pSignalSemaphores = &m_Semaphores.RenderComplete;

	VkFormat depthFormat = m_Device->GetPyhsicalDevice()->GetDepthFormat();

	// Render Pass
	VkAttachmentDescription colourAttachmentDesc = {};
	colourAttachmentDesc.format = m_ColourFormat;
	colourAttachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
	colourAttachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colourAttachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colourAttachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colourAttachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colourAttachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colourAttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorReference = {};
	colorReference.attachment = 0;
	colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthReference = {};
	depthReference.attachment = 1;
	depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpassDescription = {};
	subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpassDescription.colorAttachmentCount = 1;
	subpassDescription.pColorAttachments = &colorReference;
	subpassDescription.inputAttachmentCount = 0;
	subpassDescription.pInputAttachments = nullptr;
	subpassDescription.preserveAttachmentCount = 0;
	subpassDescription.pPreserveAttachments = nullptr;
	subpassDescription.pResolveAttachments = nullptr;

	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colourAttachmentDesc;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpassDescription;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	VK_CHECK_RESULT(vkCreateRenderPass(m_Device->GetVkDevice(), &renderPassInfo, nullptr, &m_RenderPass));

	// Create framebuffers for every swapchain image
	{
		for (auto& framebuffer : m_Framebuffers)
			vkDestroyFramebuffer(device, framebuffer, nullptr);

		VkFramebufferCreateInfo frameBufferCreateInfo = {};
		frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		frameBufferCreateInfo.renderPass = m_RenderPass;
		frameBufferCreateInfo.attachmentCount = 1;
		frameBufferCreateInfo.width = m_Width;
		frameBufferCreateInfo.height = m_Height;
		frameBufferCreateInfo.layers = 1;

		m_Framebuffers.resize(m_ImageCount);
		for (uint32_t i = 0; i < m_Framebuffers.size(); i++)
		{
			frameBufferCreateInfo.pAttachments = &m_Images[i].ImageView;
			VK_CHECK_RESULT(vkCreateFramebuffer(m_Device->GetVkDevice(), &frameBufferCreateInfo, nullptr, &m_Framebuffers[i]));
		}
	}
}

void VulkanSwapChain::Destroy()
{
	
}

void VulkanSwapChain::OnResize(int width, int height)
{
	VkDevice device = m_Device->GetVkDevice();
	vkDeviceWaitIdle(device);
	Create(&width, &height);
	vkDeviceWaitIdle(device);
}

void VulkanSwapChain::BeginFrame()
{
	PROFILE_FUNCTION();

	//TODO: resource release  queue

	VK_CHECK_RESULT(vkResetCommandPool(m_Device->GetVkDevice(), m_CommandBuffers[m_CurrentBufferIndex].CommandPool, 0));
}

void VulkanSwapChain::Present()
{
	PROFILE_FUNCTION();
	
	VkPipelineStageFlags waitStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pWaitDstStageMask = &waitStageMask;
	submitInfo.pWaitSemaphores = &m_Semaphores.PresentComplete;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &m_Semaphores.RenderComplete;
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pCommandBuffers = &m_CommandBuffers[m_CurrentBufferIndex].CommandBuffer;
	submitInfo.commandBufferCount = 1;

	VK_CHECK_RESULT(vkResetFences(m_Device->GetVkDevice(), 1, &m_WaitFences[m_CurrentBufferIndex]));
	VK_CHECK_RESULT(vkQueueSubmit(m_Device->GetGraphicsQueue(), 1, &submitInfo, m_WaitFences[m_CurrentBufferIndex]));

	VkResult result;
	{
		PROFILE_SCOPE("Vulkan Queue Present");

		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.pNext = NULL;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &m_Swapchain;
		presentInfo.pImageIndices = &m_CurrentImageIndex;
		presentInfo.pWaitSemaphores = &m_Semaphores.RenderComplete;
		presentInfo.waitSemaphoreCount = 1;
		result = vkQueuePresentKHR(m_Device->GetGraphicsQueue(), &presentInfo);
	}

	if (result != VK_SUCCESS)
	{
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
		{
			OnResize(m_Width, m_Height);
		}
		else {
			VK_CHECK_RESULT(result);
		}
	}

	{
		PROFILE_SCOPE("Vulkan wait for fences");
		m_CurrentBufferIndex = (m_CurrentBufferIndex + 1);
		VK_CHECK_RESULT(vkWaitForFences(m_Device->GetVkDevice(), 1, &m_WaitFences[m_CurrentBufferIndex], VK_TRUE, UINT64_MAX));
	}
}
