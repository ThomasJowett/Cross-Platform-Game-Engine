#include "FontView.h"

#include "Renderer/Renderer.h"
#include "Renderer/Renderer2D.h"
#include "Renderer/FrameBuffer.h"
#include "Renderer/Font.h"
#include "Renderer/Camera.h"
#include "Renderer/RenderCommand.h"

#include "MainDockSpace.h"

#include "IconsFontAwesome6.h"
#include "imgui/imgui.h"

FontView::FontView(bool* show, std::filesystem::path filepath)
	:View("FontView"), m_Show(show), m_Filepath(filepath)
{
	FrameBufferSpecification frameBufferSpecification = { 512, 256 };
	frameBufferSpecification.attachments = { FrameBufferTextureFormat::RGBA8 };
	m_Framebuffer = FrameBuffer::Create(frameBufferSpecification);

	m_String = "The quick brown fox jumps over the lazy dog\n\n!\"#$%&\'()*+,-./0123456789:;<=>?@ ABCDEFGHIJKLMNOPQRSTUVWXYZ [\\]^_`abcdefghijklmnopqrstuvwxyz{ | }~'";
}

void FontView::OnAttach()
{
	m_WindowName = ICON_FA_FONT + std::string(" " + m_Filepath.filename().string());

	m_Font = AssetManager::GetAsset<Font>(m_Filepath);
	m_Camera = CreateRef<OrthographicCamera>(-11.0f, 11.0f, -5.0f, 5.0f);
}

void FontView::OnImGuiRender()
{
	if(!*m_Show)
		return;

	if(ImGui::Begin(m_WindowName.c_str(), m_Show))
	{
		if(ImGui::IsWindowFocused())
		{
			MainDockSpace::SetFocussedWindow(this);
		}

		m_Framebuffer->Bind();
		RenderCommand::Clear();
		Renderer::BeginScene(Matrix4x4::Translate(Vector3f( 10.0f, -2.5f, 0.0f)), m_Camera->GetProjectionMatrix());
		Renderer2D::DrawString(m_String, m_Font, 20, Vector3f(0, 0, 0), 0);
		Renderer::EndScene();
		m_Framebuffer->UnBind();

		uint64_t tex = (uint64_t)m_Framebuffer->GetColourAttachment();

		ImGui::Image((void*)tex, ImVec2(512.0f, 256.0f), ImVec2(0, 1), ImVec2(1,0));
	}

	ImGui::End();
}
