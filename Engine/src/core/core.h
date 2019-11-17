#pragma once

#ifdef __WINDOWS__ //windows x64 & x86

#elif __linux__ 

#elif __APPLE__

#elif __ANDROID__

#else
#error Target platform not supported
#endif

#ifdef ENABLE_ASSERTS
	#define ASSERT(x, ...) {if(!(x)){__debugbreak();}}
	#define CORE_ASSERT(x,...){if(!(x)){__debugbreak();}}
#else
	#define ASSERT(x, ...)
	#define CORE_ASSERT(z, ...)
#endif // ENABLE_ASSERTS


#define BIT(x) (1 << x)

#define BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)