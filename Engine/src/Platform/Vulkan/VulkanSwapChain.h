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
private:
	VkInstance m_Instance;
	Ref<VulkanDevice> m_Device;
	VkSurfaceKHR m_Surface = VK_NULL_HANDLE;

	VkSwapchainKHR m_Swapchain = VK_NULL_HANDLE;

	bool m_VSync;
};
