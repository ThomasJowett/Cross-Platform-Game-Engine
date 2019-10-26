#pragma once

#ifdef JPT_PLATFORM_WINDOWS
#ifdef JPT_BUILD_DLL
#define JUPITER_API __declspec(dllexport)
#else 
#define JUPITER_API __declspec(dllimport)
#endif // JPT_BUILD_DLL
#else
#error Target platform not supported
#endif // JPT_PLATFORM_WINDOWS
