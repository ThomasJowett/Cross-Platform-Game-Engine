#include "AudioView.h"
#include <Scene/AssetManager.h>
#include <IconsFontAwesome6.h>
#include <imgui.h>
#include <MainDockSpace.h>
#include <Asset/Audio.h>

AudioView::AudioView(bool* show, std::filesystem::path filepath)
	:View("AudioView"), m_Show(show), m_Filepath(filepath)
{

}

void AudioView::OnAttach()
{
	m_WindowName = ICON_FA_FILE_AUDIO + std::string(" " + m_Filepath.filename().string());
	m_Audio = AssetManager::GetAsset<AudioClip>(m_Filepath);
}

void AudioView::OnImGuiRender()
{
	if (!*m_Show) {
		ViewerManager::CloseViewer(m_Filepath);
		return;
	}
	if (ImGui::Begin(m_WindowName.c_str(), m_Show))
	{
		if (ImGui::IsWindowFocused())
		{
			MainDockSpace::SetFocussedWindow(this);
		}
		ImGui::Text("Sample Rate: %d", m_Audio->GetSampleRate());
		ImGui::Text("Channels: %d", m_Audio->GetChannels());
		ImGui::Text("Bit Depth: %d", m_Audio->GetBitDepth());
	}
	ImGui::End();
}
