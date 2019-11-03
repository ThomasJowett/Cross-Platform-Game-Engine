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
		#define DLLIMPEXP_CORE
	#endif // BUILD_DLL
#else
#error Target platform not supported
#endif

#define BIT(X) (1 << X)