#include "FontView.h"

#include "Renderer/Renderer.h"
#include "Renderer/Renderer2D.h"
#include "Renderer/FrameBuffer.h"
#include "Asset/Font.h"
#include "Renderer/Camera.h"
#include "Renderer/RenderCommand.h"

#include "MainDockSpace.h"

#include "IconsFontAwesome6.h"
#include "imgui/imgui.h"
#include "ImGui/ImGuiUtilities.h"

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
	if (!*m_Show) {
		ViewerManager::CloseViewer(m_Filepath);
		return;
	}

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

		ImGui::Image(m_Framebuffer->GetColourAttachment(), ImVec2(512.0f, 256.0f));
	}

	ImGui::End();
}
