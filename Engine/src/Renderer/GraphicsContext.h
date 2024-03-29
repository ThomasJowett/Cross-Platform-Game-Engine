#pragma once

class GraphicsContext
{
public:
	virtual ~GraphicsContext() = default;

	virtual void Init() = 0;
	virtual void SwapBuffers() = 0;
	virtual void ResizeBuffers(uint32_t width, uint32_t height) = 0;
	virtual void SetSwapInterval(uint32_t interval) = 0;
	virtual void MakeCurrent() = 0;
};