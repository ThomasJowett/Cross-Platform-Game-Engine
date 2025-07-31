#pragma once

#ifdef ATCP_ENABLE_TRACY
#ifndef TRACY_ENABLE
#define TRACY_ENABLE
#endif

#include <Tracy.hpp>

// Resolve which function signature macro will be used. Note that this only
// is resolved when the (pre)compiler starts, so the syntax highlighting
// could mark the wrong one in your editor!
#if defined(__clang__) || defined(__GNUC__)
    // Clang or GCC
#define FUNC_SIG __PRETTY_FUNCTION__

#elif defined(_MSC_VER)
    // Microsoft Visual C++
#define FUNC_SIG __FUNCSIG__

#elif defined(__INTEL_COMPILER)
    // Intel Compiler
#define FUNC_SIG __FUNCTION__

#elif defined(__DMC__) && (__DMC__ >= 0x810)
    // Digital Mars Compiler
#define FUNC_SIG __PRETTY_FUNCTION__

#elif defined(__BORLANDC__) && (__BORLANDC__ >= 0x550)
    // Borland Compiler
#define FUNC_SIG __FUNC__

#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
    // C99 standard
#define FUNC_SIG __func__

#elif defined(__cplusplus) && (__cplusplus >= 201103L)
    // C++11 or later
#define FUNC_SIG __func__

#else
#error "Could not determine the function signature macro for this compiler. Please define FUNC_SIG manually."
#endif

#define PROFILE_FRAME() FrameMark
#define PROFILE_SCOPE(name) ZoneScopedN(name)
#define PROFILE_FUNCTION() PROFILE_SCOPE(FUNC_SIG)
#define PROFILE_TAG(x, y) ZoneText(x, strlen(x))
#define PROFILE_LOG(text, size) TracyMessage(text, size)
#define PROFILE_VALUE(text, value) TracyPlot(text, value)
#define PROFILE_ALLOC(ptr, size) TracyAlloc(ptr, size)
#define PROFILE_FREE(ptr) TracyFree(ptr)
#else

#define PROFILE_FRAME()
#define PROFILE_FUNCTION()
#define PROFILE_SCOPE(name)
#define PROFILE_TAG(x, y)
#define PROFILE_LOG(text, size)
#define PROFILE_VALUE(text, value)
#define PROFILE_ALLOC(ptr, size)
#define PROFILE_FREE(ptr)
#endif // ENABLE_PROFILING
