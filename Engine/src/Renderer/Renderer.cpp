#include "stdafx.h"
#include "Renderer.h"

RendererAPI Renderer::s_RendererAPI = RendererAPI::OpenGL;

void Renderer::SetAPI(RendererAPI api)
{
	s_RendererAPI = api;
}
