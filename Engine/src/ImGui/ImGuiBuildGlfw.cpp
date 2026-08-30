#pragma warning(disable:4996)

#ifndef __EMSCRIPTEN__
#define IMGUI_IMPL_WEBGPU_BACKEND_WGPU
#endif // __EMSCRIPTEN__

#include <imgui/backends/imgui_impl_glfw.cpp>
#include <imgui/backends/imgui_impl_opengl3.cpp>
#include <imgui/backends/imgui_impl_wgpu.cpp>
