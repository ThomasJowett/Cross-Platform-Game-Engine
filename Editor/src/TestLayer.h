#pragma once

#include "include.h"

class TestLayer :public Layer
{
public:
	TestLayer(Ref<FrameBuffer> framebuffer);
	~TestLayer() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	virtual void OnUpdate(float deltaTime) override;
	virtual void OnFixedUpdate() override;
	virtual void OnEvent(Event& e) override;

private:
	ShaderLibrary m_ShaderLibrary;
	PerspectiveCameraController m_CameraController;
	Ref<VertexArray> m_TorusVertexArray;

	Ref<Texture2D> m_Texture;

	Ref<FrameBuffer> m_Framebuffer;
};