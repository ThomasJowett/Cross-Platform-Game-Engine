#include "FontView.h"

#include "MainDockSpace.h"

#include "IconsFontAwesome6.h"
#include "imgui/imgui.h"

FontView::FontView(bool* show, std::filesystem::path filepath)
	:View("FontView"), m_Show(show), m_Filepath(filepath)
{
}

void FontView::OnAttach()
{
	m_WindowName = ICON_FA_FONT + std::string(" " + m_Filepath.filename().string());
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

		uint64_t tex = (uint64_t)m_Framebuffer->GetColourAttachment();

		ImGui::Image((void*)tex, ImVec2(512.0f, 512.0f), ImVec2(0, 1), ImVec2(1,0));
	}
}
