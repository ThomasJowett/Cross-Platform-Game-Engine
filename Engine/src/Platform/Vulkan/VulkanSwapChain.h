#include "Vulkan.h"
#include "VulkanDevice.h"

struct GLFWwindow;

class VulkanSwapChain
{
public:
	VulkanSwapChain() = default;

	void Init(VkInstance instance, Ref<VulkanDevice> device);
	void InitSurface(GLFWwindow* windowHandle);
	void Create(int* width, int* height);
	void Destroy();

	void OnResize(int width, int height);

	void BeginFrame();
	void Present();

	uint32_t GetImageCount() const { return m_ImageCount; }
	VkRenderPass GetRenderPass() { return m_RenderPass; }

	uint32_t GetCurrentBufferIndex() const { return m_CurrentBufferIndex; }

	uint32_t GetWidth() const { return m_Width; }
	uint32_t GetHeight() const { return m_Height; }

	VkCommandBuffer GetCurrentDrawCommandBuffer() { return GetDrawCommandBuffer(m_CurrentBufferIndex); }
	VkCommandBuffer GetDrawCommandBuffer(uint32_t index)
	{
		ASSERT(index < m_CommandBuffers.size(), "Command buffer out of range");
		return m_CommandBuffers.at(index).CommandBuffer;
	}

	VkFramebuffer GetCurrentFramebuffer() { return GetFramebuffer(m_CurrentImageIndex); }
	VkFramebuffer GetFramebuffer(uint32_t index)
	{
		ASSERT(index < m_Framebuffers.size(), "Frame buffer out of range");
		return m_Framebuffers.at(index);
	}

	VkCommandPool GetCurrentCommandPool() { return GetCommandPool(m_CurrentBufferIndex); }
	VkCommandPool GetCommandPool(uint32_t index)
	{
		ASSERT(index < m_CommandBuffers.size(), "Command buffer out of range");
		return m_CommandBuffers.at(index).CommandPool;
	}
private:
	VkInstance m_Instance;
	Ref<VulkanDevice> m_Device;
	VkSurfaceKHR m_Surface = VK_NULL_HANDLE;

	VkSwapchainKHR m_Swapchain = VK_NULL_HANDLE;

	uint32_t m_ImageCount = 0;

	VkRenderPass m_RenderPass = nullptr;

	uint32_t m_Width = 0;
	uint32_t m_Height = 0;

	VkFormat m_ColourFormat;
	VkColorSpaceKHR m_ColourSpace;

	struct SwapChainImage
	{
		VkImage Image = nullptr;
		VkImageView ImageView = nullptr;
	};
	std::vector<SwapChainImage> m_Images;
	std::vector<VkImage> m_VulkanImages;

	struct
	{
		VkSemaphore PresentComplete = nullptr;
		VkSemaphore RenderComplete = nullptr;
	}m_Semaphores;

	VkSubmitInfo m_SubmitInfo;

	std::vector<VkFence> m_WaitFences;

	std::vector<VkFramebuffer> m_Framebuffers;

	uint32_t m_CurrentBufferIndex = 0;
	uint32_t m_CurrentImageIndex = 0;

	struct SwapChainCommandBuffer
	{
		VkCommandPool CommandPool = nullptr;
		VkCommandBuffer CommandBuffer = nullptr;
	};
	std::vector<SwapChainCommandBuffer> m_CommandBuffers;
};
