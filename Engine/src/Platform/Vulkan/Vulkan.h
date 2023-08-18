#pragma once

#define VK_NO_PROTOTYPES
#include <volk.h>

#define VK_CHECK_RESULT(f)\
{\
	VkResult result = (f);\
	if (result != VK_SUCCESS)\
	{\
		ENGINE_ERROR("VkResult is '{0}' in {1}:{2}", result, __FILE__, __LINE__);\
	}\
}