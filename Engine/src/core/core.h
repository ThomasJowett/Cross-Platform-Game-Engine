#pragma once

#ifdef JPT_PLATFORM_WINDOWS //windows x64 & x86
	#ifdef JPT_BUILD_DLL
		#define JUPITER_API __declspec(dllexport)
	#else 
		#define JUPITER_API __declspec(dllimport)
	#endif // JPT_BUILD_DLL
#elif __linux__ 
	#ifdef JPT_BUILD_DLL
		#define JUPITER_API __attribute__((visibility("default")))
	#else
		#define JUPITER_API
	#endif // JPT_BUILD_DLL
#elif __APPLE__
	#ifdef JPT_BUILD_DLL
		#define JUPITER_API
	#else
		#define JUPITER_API
	#endif // JPT_BUILD_DLL
#else
#error Target platform not supported
#endif
