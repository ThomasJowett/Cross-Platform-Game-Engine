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
	std::filesystem::path absolutePath = std::filesystem::absolute(Application::GetOpenDocumentDirectory() / m_Filepath);
	if (ma_sound_init_from_file(m_AudioEngine.get(), absolutePath.string().c_str(), flags, NULL, NULL, m_Sound.get()) != MA_SUCCESS)
	{
		ENGINE_ERROR("Failed to create sound for audio file: {0}", absolutePath);
		return;
	}

	ma_sound_set_spatialization_enabled(m_Sound.get(), false);

	ma_sound_get_length_in_seconds(m_Sound.get(), &m_Length);
	ma_sound_get_cursor_in_seconds(m_Sound.get(), &m_PlaybackTime);

	ma_decoder decoder;
	if (ma_decoder_init_file(absolutePath.string().c_str(), NULL, &decoder) != MA_SUCCESS)
	{
		ENGINE_ERROR("Failed to decode audio file: {0}", absolutePath);
		return;
	}

	constexpr size_t CHUNK_SIZE = 1024;

	void* buffer = malloc(CHUNK_SIZE * decoder.outputChannels * ma_get_bytes_per_sample(decoder.outputFormat));
	std::vector<float> floatBuffer(CHUNK_SIZE * decoder.outputChannels);

	m_VolumeHistory.resize(VOLUME_HISTORY_LENGTH);
	ma_uint64 totalFramesRead = 0;
	
	ma_uint64 frameCount = 0;
	if (ma_decoder_get_length_in_pcm_frames(&decoder, &frameCount) != MA_SUCCESS) {
		ENGINE_ERROR("Failed to get frame count for audio file: {0}", m_Filepath);
		return;
	}

	int channels = decoder.outputChannels;

	ma_uint64 framesPerSample = frameCount / VOLUME_HISTORY_LENGTH;

	for (size_t i = 0; i < VOLUME_HISTORY_LENGTH; i++)
	{
		ma_uint64 seekFrame = i * framesPerSample;

		if (ma_decoder_seek_to_pcm_frame(&decoder, seekFrame) != MA_SUCCESS)
		{
			ENGINE_ERROR("Failed to seek to frame {0} for audio file: {1}", seekFrame, m_Filepath);
			break;
		}

		ma_uint64 framesRead = 0;
		if (ma_decoder_read_pcm_frames(&decoder, buffer, CHUNK_SIZE, &framesRead) != MA_SUCCESS) {
			ENGINE_ERROR("Error reading PCM frames for audio file: {0}, at frame {1}", m_Filepath, totalFramesRead);
			break;
		}

		if (framesRead == 0)
		{
			break;
		}

		if (decoder.outputFormat == ma_format_f32)
		{
			memcpy(floatBuffer.data(), buffer, framesRead * channels * sizeof(float));
		}
		else
		{
			ma_pcm_convert(floatBuffer.data(), ma_format_f32, buffer, decoder.outputFormat, framesRead * decoder.outputChannels, ma_dither_mode_none);
		}

		float rms = 0.0f;
		for (ma_uint64 i = 0; i < framesRead * channels; i += channels)
		{
			rms += floatBuffer[i] * floatBuffer[i];
		}
		rms = sqrt(rms / framesRead);
		m_VolumeHistory[i] = rms;
	}

	free(buffer);
	ma_decoder_uninit(&decoder);
}

void AudioView::OnImGuiRender()
{
	if (!*m_Show) {
		ViewerManager::CloseViewer(m_Filepath);
		return;
	}

	ImGui::SetNextWindowSize(ImVec2(800, 280), ImGuiCond_FirstUseEver);

	if (ImGui::Begin(m_WindowName.c_str(), m_Show))
	{
		ma_sound_get_cursor_in_seconds(m_Sound.get(), &m_PlaybackTime);

		float fullWidth = ImGui::GetContentRegionAvail().x;
		float plotHeight = 120.0f;
		
		if (ImGui::IsWindowFocused())
		{
			MainDockSpace::SetFocussedWindow(this);
		}
		ImGui::Text("Sample Rate: %d", m_Audio->GetSampleRate());
		ImGui::Text("Channels: %d", m_Audio->GetChannels());
		ImGui::Text("Bit Depth: %d", m_Audio->GetBitDepth());

		ImGui::PushItemWidth(fullWidth);
		ImVec2 plotPos = ImGui::GetCursorScreenPos();
		ImVec2 plotSize = ImVec2(fullWidth, plotHeight);
		ImGui::PlotHistogram("##Volume", m_VolumeHistory.data(), VOLUME_HISTORY_LENGTH, 0, NULL, 0.0f, 1.0f, plotSize);

		ImDrawList* drawList = ImGui::GetWindowDrawList();
		float playbackNorm = m_PlaybackTime / m_Length;
		float playbackX = plotPos.x + playbackNorm * plotSize.x;

		drawList->AddLine(ImVec2(playbackX, plotPos.y),
			ImVec2(playbackX, plotPos.y + plotSize.y),
			ImGui::GetColorU32(ImGuiCol_Text), 2.0f);

		if (ImGui::SliderFloat("##PlaybackTime", &m_PlaybackTime, 0.0f, m_Length, "%.3fs")) {
			if (m_Playing)
				ma_sound_stop(m_Sound.get());
			ma_sound_seek_to_second(m_Sound.get(), m_PlaybackTime);
		}
		
		if (!ImGui::IsItemActive() && m_Playing) {
			ma_sound_start(m_Sound.get());
			m_Playing = true;
		}
		ImGui::PopItemWidth();

		ImGui::Dummy(ImVec2(0,0));

		ImGui::SameLine(ImGui::GetWindowContentRegionMax().x / 2.0f);

		if (m_Playing) {
			if (ImGui::Button(ICON_FA_PAUSE "##Pause"))
			{
				ma_sound_stop(m_Sound.get());
				m_Playing = false;
			}
		}
		else {
			if (ImGui::Button(ICON_FA_PLAY "##Play"))
			{
				ma_sound_start(m_Sound.get());
				m_Playing = true;
			}
		}

		if (m_PlaybackTime == m_Length)
		{
			ma_sound_stop(m_Sound.get());
			ma_sound_seek_to_second(m_Sound.get(), 0.0f);
			m_Playing = false;
		}
	}
	ImGui::End();
}
