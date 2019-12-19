#include "stdafx.h"
#include "OpenGLRendererAPI.h"

#include <GLAD/glad.h>

bool OpenGLRendererAPI::Init()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	return false;
}

void OpenGLRendererAPI::SetClearColour(float red, float green, float blue, float alpha)
{
	glClearColor(red, green, blue, alpha);
}

void OpenGLRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
	glViewport(x, y, width, height);
}

void OpenGLRendererAPI::Clear()
{
	glClear(GL_COLOR_BUFFER_BIT |GL_DEPTH_BUFFER_BIT);
}

void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray)
{
	vertexArray->Bind();
	glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
}
