#pragma once
#include <memory>

#ifdef __WINDOWS__ //windows x64 & x86

#elif defined(__linux__)

#elif defined(__APPLE__) || defined(__MACH__)
	#include <TargetConditionals.h>
	#if TARGET_IPHONE_SIMULATOR == 1
		#error "IOS simulator is not yet supported!"
	#elif TARGET_OS_IPHONE == 1
		#define PLATFORM_IOS
		#error "IOS is not yet supported!"
	#elif TARGET_OS_MAC == 1
		#define TARGET_OS_MAC == 1
		#define PLATFORM_MACOS
	#else
		#error "Unkown Apple Platform!"
#endif

#elif defined(__ANDROID__)
	#error "Android is not supported!"

#else
	#error Target platform not supported
#endif

#ifdef DEBUG
	#if defined(_MSC_VER)
		#define DEBUGBREAK() __debugbreak()
	#elif defined(__linux__)
		#include <signal.h>
		#if defined(SIGTRAP)
			#define DEBUGBREAK() raise(SIGTRAP)
		#else
			#define DEBUGBREAK() raise(SIGABRT)
		#endif
	#elif defined(__APPLE__)
		#define DEBUGBREAK() __builtin_trap()
	#else
		#define DEBUGBREAK()
	#endif
#else
	#define DEBUGBREAK()
#endif // DEBUG

#define EXPAND_MACRO(x) x
#define STRINGIFY_MACRO(x) #x

#ifdef ENABLE_ASSERTS
	#define ASSERT(x, ...) {if(!(x)){CLIENT_ERROR("Assertion failed: {0}", __VA_ARGS__);DEBUGBREAK();}}
	#define CORE_ASSERT(x,...){if(!(x)){ENGINE_ERROR("Assertion failed: {0}", __VA_ARGS__);DEBUGBREAK();}}
#else
	#define ASSERT(x, ...)
	#define CORE_ASSERT(z, ...)
#endif // ENABLE_ASSERTS

#if defined(_MSC_VER)
#define ALIGNED_(x) __declspec(align(x))
#else
#if defined(__GNUC__)
#define ALIGNED_(x) __attribute__ ((aligned(x)))
#endif
#endif

#define ALIGNED_TYPE(t,x) typedef t ALIGNED_(x)


#define BIT(x) (1 << x)

#define BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

template<typename T>
using Scope = std::unique_ptr<T>;
template<typename T, typename ... Args>
constexpr Scope<T> CreateScope(Args&& ... args)
{
	return std::make_unique<T>(std::forward<Args>(args)...);
}

template<typename T>
using Ref = std::shared_ptr<T>;
template<typename T, typename ... Args>
constexpr Ref<T> CreateRef(Args&& ... args)
{
	return std::make_shared<T>(std::forward<Args>(args)...);
}

#include "Logging/Logger.h"