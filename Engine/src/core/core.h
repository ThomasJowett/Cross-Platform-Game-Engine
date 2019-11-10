#pragma once

#ifdef __WINDOWS__ //windows x64 & x86
	#ifdef BUILD_DLL
		#define DLLIMPEXP_CORE __declspec(dllexport)
	#else 
		#define DLLIMPEXP_CORE __declspec(dllimport)
	#endif // BUILD_DLL
#elif __linux__ 
	#ifdef BUILD_DLL
		#define DLLIMPEXP_CORE __attribute__((visibility("default")))
	#else
		#define DLLIMPEXP_CORE
	#endif // BUILD_DLL
#elif __APPLE__
	#ifdef BUILD_DLL
		#define DLLIMPEXP_CORE
	#else
		#define DLLIMPEXP_CORE __attribute__((visibility("default")))
	#endif // BUILD_DLL
#elif __ANDROID__
	#ifdef BUILD_DLL
		#define DLLIMPEXP_CORE
	#else
		#define DLLIMPEXP_CORE
	#endif // BUILD_DLL
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