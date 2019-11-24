#pragma once

enum class RendererAPI
{
	None = 0,
	OpenGL = 1,
	Directx11,
	Metal,
	Vulkan
};

class Renderer
{
public:
	inline static RendererAPI GetAPI() { return s_RendererAPI; }
	static void SetAPI(RendererAPI api);
	static RendererAPI s_RendererAPI;
};