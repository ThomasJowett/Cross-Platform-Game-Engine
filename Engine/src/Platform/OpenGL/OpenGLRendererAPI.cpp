#include "stdafx.h"
#include "OpenGLRendererAPI.h"

#include <glad/glad.h>

void OpenGLMessageCallback(unsigned source, unsigned type, unsigned id, unsigned severity,
	int length, const char* message, const void* userParam)
{
	if (type == GL_DEBUG_TYPE_ERROR)
	{
		ENGINE_ERROR("GL CALLBACK: type = 0x{0} severity = 0x{1} message = {2}", std::to_string(type), std::to_string(severity), std::string(message));
	}
	else
	{
		ENGINE_WARN("GL CALLBACK: {0}", message);
	}
}

bool OpenGLRendererAPI::Init()
{
	PROFILE_FUNCTION();

#ifdef DEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(OpenGLMessageCallback, nullptr);

	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
#endif // DEBUG

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glLineWidth(3.0f);
	glEnable(GL_LINE_SMOOTH);
	return true;
}

void OpenGLRendererAPI::SetClearColour(const Colour& colour)
{
	glClearColor(colour.r, colour.g, colour.b, colour.a);
}

void OpenGLRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
	glViewport(x, y, width, height);
}

void OpenGLRendererAPI::Clear()
{
	glClear(GL_COLOR_BUFFER_BIT |GL_DEPTH_BUFFER_BIT);
}

void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount, DrawMode drawMode)
{
	vertexArray->Bind();
	GLuint mode;
	switch (drawMode)
	{
	case DrawMode::POINTS: mode = GL_POINTS; break;
	case DrawMode::WIREFRAME: mode = GL_LINES; break;
	case DrawMode::FILL: mode = GL_TRIANGLES; break;
	default: mode = GL_TRIANGLES; break;
	}

	uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetCount();

	glDrawElements(mode, count, GL_UNSIGNED_INT, nullptr);
}

void OpenGLRendererAPI::DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount)
{
	vertexArray->Bind();
	glDrawArrays(GL_LINES, 0, vertexCount);
}
