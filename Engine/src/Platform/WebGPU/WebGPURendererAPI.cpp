#include "stdafx.h"
#include "WebGPURendererAPI.h"
#include "Logging/Instrumentor.h"

#include <glad/glad.h>

void WebGPUMessageCallback(unsigned source, unsigned type, unsigned id, unsigned severity,
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

bool WebGPURendererAPI::Init()
{
	PROFILE_FUNCTION();

#ifdef DEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(WebGPUMessageCallback, nullptr);

	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
#endif // DEBUG

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glLineWidth(1.5f);
	return true;
}

void WebGPURendererAPI::SetClearColour(const Colour& colour)
{
	glClearColor(colour.r, colour.g, colour.b, colour.a);
}

void WebGPURendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
	glViewport(x, y, width, height);
}

void WebGPURendererAPI::Clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void WebGPURendererAPI::ClearColour()
{
	glClear(GL_COLOR_BUFFER_BIT);
}

void WebGPURendererAPI::ClearDepth()
{
	glClear(GL_DEPTH_BUFFER_BIT);
}

void WebGPURendererAPI::DrawIndexed(uint32_t indexCount, uint32_t indexStart, uint32_t vertexOffset, bool backFaceCull, DrawMode drawMode)
{
	if (!backFaceCull)
		glDisable(GL_CULL_FACE);

	GLuint mode;
	switch (drawMode)
	{
	case DrawMode::POINTS: mode = GL_POINTS; break;
	case DrawMode::WIREFRAME: mode = GL_LINES; break;
	case DrawMode::FILL: mode = GL_TRIANGLES; break;
	default: mode = GL_TRIANGLES; break;
	}

	glDrawRangeElementsBaseVertex(mode, indexStart, indexStart + indexCount, indexCount, GL_UNSIGNED_INT, nullptr, vertexOffset);
	if (!backFaceCull)
		glEnable(GL_CULL_FACE);
}

void WebGPURendererAPI::DrawLines(uint32_t vertexCount)
{
	glDrawArrays(GL_LINES, 0, vertexCount);
}