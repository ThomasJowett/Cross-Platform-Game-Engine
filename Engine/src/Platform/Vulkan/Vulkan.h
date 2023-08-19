#pragma once

#define VK_NO_PROTOTYPES
#include <volk.h>

#include <vulkan/vk_enum_string_helper.h >

#define VK_CHECK_RESULT(f)\
{\
	VkResult result = (f);\
	if (result != VK_SUCCESS)\
	{\
		ENGINE_ERROR("VkResult is '{0}' in {1}:{2}", string_VkResult(result), __FILE__, __LINE__); \
	}\
}
