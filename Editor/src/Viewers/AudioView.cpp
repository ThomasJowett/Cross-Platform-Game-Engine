#include "AudioView.h"
#include <Scene/AssetManager.h>
#include <IconsFontAwesome6.h>
#include <imgui.h>
#include <miniaudio/miniaudio.h>
#include <MainDockSpace.h>
#include <Asset/AudioClip.h>

constexpr uint32_t VOLUME_HISTORY_LENGTH = 500;

AudioView::AudioView(bool* show, std::filesystem::path filepath)
	:View("AudioView"), m_Show(show), m_Filepath(filepath)
{

}

AudioView::~AudioView()
{
	ma_sound_uninit(m_Sound.get());
	ma_engine_uninit(m_AudioEngine.get());
}

void AudioView::OnAttach()
{
	m_WindowName = ICON_FA_FILE_AUDIO + std::string(" " + m_Filepath.filename().string());
	m_Audio = AssetManager::GetAsset<AudioClip>(m_Filepath);

	m_AudioEngine = CreateRef<ma_engine>();
	if (ma_engine_init(nullptr, m_AudioEngine.get()) != MA_SUCCESS)
	{
		ENGINE_ERROR("Failed to create audio engine for audio file: {0}", m_Filepath);
		return;
	}

	m_Sound = CreateRef<ma_sound>();
	ma_uint32 flags = MA_RESOURCE_MANAGER_DATA_SOURCE_FLAG_DECODE;
	if (ma_sound_init_from_file(m_AudioEngine.get(), m_Filepath.string().c_str(), flags, NULL, NULL, m_Sound.get()) != MA_SUCCESS)
	{
		ENGINE_ERROR("Failed to create sound for audio file: {0}", m_Filepath);
		return;
	}

	ma_sound_set_spatialization_enabled(m_Sound.get(), false);

	ma_sound_get_length_in_seconds(m_Sound.get(), &m_Length);
	ma_sound_get_cursor_in_seconds(m_Sound.get(), &m_PlaybackTime);

	ma_decoder decoder;
	if (ma_decoder_init_file(m_Filepath.string().c_str(), NULL, &decoder) != MA_SUCCESS)
	{
		ENGINE_ERROR("Failed to decode audio file: {0}", m_Filepath);
		return;
	}

	constexpr size_t CHUNK_SIZE = 4096;

	float* buffer = new float[CHUNK_SIZE];

	m_VolumeHistory.resize(VOLUME_HISTORY_LENGTH);
	uint32_t volumeHistoryIndex = 0;
	ma_uint64 totalFramesRead = 0;
	
	ma_uint64 frameCount = 0;
	if (ma_decoder_get_length_in_pcm_frames(&decoder, &frameCount) != MA_SUCCESS) {
		ENGINE_ERROR("Failed to get frame count for audio file: {0}", m_Filepath);
		return;
	}

	while (totalFramesRead < frameCount)
	{
		ma_uint64 framesRead = 0;
		ma_decoder_read_pcm_frames(&decoder, buffer, CHUNK_SIZE, &framesRead);

		if (framesRead == 0)
		{
			break;
		}

		float rms = 0.0f;
		for (ma_uint64 i = 0; i < framesRead; i++)
		{
			rms += buffer[i] * buffer[i];
		}
		rms = sqrt(rms / framesRead);
		m_VolumeHistory[volumeHistoryIndex] = rms;
		volumeHistoryIndex = (volumeHistoryIndex + 1) % VOLUME_HISTORY_LENGTH;

		totalFramesRead += framesRead;
	}

	delete[] buffer;
	ma_decoder_uninit(&decoder);
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
