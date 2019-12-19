#pragma once
#include <include.h>
#include "Platform/OpenGL/OpenGLShader.h"
#include "imgui/imgui.h"

class ExampleLayer3D :public Layer
{
public:
	ExampleLayer3D()
		:Layer("Example"), m_CameraController(16.0f / 9.0f)
	{
		m_VertexArray = VertexArray::Create();

		float vertices[] =
		{
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f
		};

		unsigned int indices[] = { 0,1,2, 0,2,3 };

		Ref<VertexBuffer> vertexBuffer;
		vertexBuffer = VertexBuffer::Create(vertices, sizeof(vertices));
		Ref<IndexBuffer> indexBuffer;
		indexBuffer = IndexBuffer::Create(indices, 6);

		BufferLayout layout = {
			{ShaderDataType::Float3, "a_position"},
			{ShaderDataType::Float2, "a_texCoord"}
		};

		vertexBuffer->SetLayout(layout);

		m_VertexArray->AddVertexBuffer(vertexBuffer);
		m_VertexArray->SetIndexBuffer(indexBuffer);

		Ref<Shader> shader = m_ShaderLibrary.Load("BasicShader");
		m_Texture = Texture2D::Create("resources/circle.png");

		std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformInteger("u_texture", 0);
	}

	void OnUpdate(float deltaTime) override
	{
		Ref<Shader> shader = m_ShaderLibrary.Get("BasicShader");
		std::dynamic_pointer_cast<OpenGLShader>(shader)->Bind();

		m_CameraController.OnUpdate(deltaTime);

		RenderCommand::SetClearColour(0.4f, 0.4f, 0.4f, 1.0f);
		RenderCommand::Clear();

		std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformFloat4("u_colour", m_Colour[0], m_Colour[1], m_Colour[2], m_Colour[4]);

		Renderer::BeginScene(m_CameraController.GetCamera());
		m_Texture->Bind();
		Renderer::Submit(shader, m_VertexArray, Matrix4x4::Scale({ 1.0f, 1.0f, 1.0f }));
		Renderer::EndScene();
	}

	void OnEvent(Event& e) override
	{
		m_CameraController.OnEvent(e);
	}

	void OnImGuiRender() override
	{
		ImGui::Begin("Settings");
		ImGui::Text(std::to_string(m_CameraController.GetZoom()).c_str());
		ImGui::ColorEdit4("Square Colour", m_Colour);
		ImGui::End();
	}

private:
	ShaderLibrary m_ShaderLibrary;
	OrthogrpahicCameraController m_CameraController;
	Ref<VertexArray> m_VertexArray;

	Ref<Texture2D> m_Texture;

	float m_Colour[4] = { 1.0f,1.0f,1.0f, 1.0f };
};