#include "TestLayer.h"

TestLayer::TestLayer(Ref<FrameBuffer> framebuffer)
	:Layer("Test Layer"), m_Framebuffer(framebuffer)
{
}

void TestLayer::OnAttach()
{
	m_TorusVertexArray = GeometryGenerator::CreateTorus(0.7f, 0.25f, 30);

	m_ShaderLibrary.Load("NormalMap");
	m_Texture = Texture2D::Create("resources/UVChecker.png");

	//m_CameraController.SetPosition({ 0.0, 0.0, 2.0 });

	// Test usage of the logger
	CLIENT_CRITICAL("[Test Critical] The game is about to crash!");
	CLIENT_ERROR("[Test Error] Something went wrong!");
	CLIENT_WARN("[Test Warning] Something may have gone wrong...");
	CLIENT_INFO("[Test Information] Normal stuff happened");
	CLIENT_DEBUG("[Test Debug] Diagnostic information");
	CLIENT_TRACE("[Test Trace] Very fine detailed diagnostic information");
}

void TestLayer::OnDetach()
{
}

void TestLayer::OnUpdate(float deltaTime)
{
	PROFILE_FUNCTION();

	m_Framebuffer->Bind();
	RenderCommand::Clear();

	Ref<Shader> shader = m_ShaderLibrary.Get("NormalMap");
	shader->Bind();

	shader->SetInt("u_texture", 0);
	shader->SetFloat4("u_colour", 1.0f, 1.0f, 1.0f, 1.0f);
	shader->SetFloat("u_tilingFactor", 1.0f);

	m_CameraController.OnUpdate(deltaTime);

	Renderer::BeginScene(m_CameraController.GetCamera());
	m_Texture->Bind();

	//Torus
	Renderer::Submit(shader, m_TorusVertexArray, Matrix4x4::Translate({ 0.0f, 0.0f,0.0f })
		* Matrix4x4::Rotate(Vector3f(PI / 2.0f, 0.0f, 0.0f)));

	Renderer::EndScene();
	m_Framebuffer->UnBind();
}

void TestLayer::OnFixedUpdate()
{
}

void TestLayer::OnEvent(Event& e)
{
	m_CameraController.OnEvent(e);

	//m_CameraController.SetAspectRatio(m_Framebuffer->GetSpecification().Width / m_Framebuffer->GetSpecification().Height);
}
