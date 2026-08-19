#include "OpenGLRendererAPI.h"
#include "Logging/Instrumentor.h"

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
	glEnable(GL_MULTISAMPLE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glLineWidth(1.5f);

	// Matches WebGPU/D3D/Metal's [0,1] clip-space Z convention, which the projection matrices
	// (Matrix4x4::OrthographicRH/PerspectiveRH) already produce unconditionally for every backend.
	glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);

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
	// A pipeline bound earlier in the frame (e.g. a depthTest=false overlay/post-process pass)
	// may have left the depth mask disabled - glClear respects it, so force it on or the depth
	// buffer silently fails to clear here.
	glDepthMask(GL_TRUE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLRendererAPI::ClearColour()
{
	glClear(GL_COLOR_BUFFER_BIT);
}

void OpenGLRendererAPI::ClearDepth()
{
	glDepthMask(GL_TRUE);
	glClear(GL_DEPTH_BUFFER_BIT);
}

void OpenGLRendererAPI::StartRenderPass(bool clear)
{
}

void OpenGLRendererAPI::EndRenderPass()
{
}

void OpenGLRendererAPI::DrawIndexed(uint32_t indexCount, uint32_t indexStart, uint32_t vertexOffset)
{
	glDrawRangeElementsBaseVertex(GL_TRIANGLES, indexStart, indexStart + indexCount, indexCount, GL_UNSIGNED_INT, nullptr, vertexOffset);
}

void OpenGLRendererAPI::DrawLines(uint32_t vertexCount)
{
	glDrawArrays(GL_LINES, 0, vertexCount);
}