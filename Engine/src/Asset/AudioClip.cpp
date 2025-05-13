#include "AudioClip.h"

#include "miniaudio/miniaudio.h"
#include "Logging/Instrumentor.h"

AudioClip::AudioClip()
{
}

AudioClip::AudioClip(const std::filesystem::path& filepath)
{
	Load(filepath);
}

AudioClip::AudioClip(const std::filesystem::path& filepath, const std::vector<uint8_t>& data)
{
	Load(filepath, data);
}

AudioClip::~AudioClip()
{
}

bool AudioClip::Load(const std::filesystem::path& filepath)
{
	PROFILE_FUNCTION();
	std::filesystem::path absolutePath = std::filesystem::absolute(Application::GetOpenDocumentDirectory() / filepath);
	if (!std::filesystem::exists(absolutePath))
	{
		ENGINE_ERROR("Audio file does not exist: {0}", absolutePath);
		return false;
	}

	ma_decoder decoder;
	if (ma_decoder_init_file(absolutePath.string().c_str(), NULL, &decoder) != MA_SUCCESS)
	{
		ENGINE_ERROR("Failed to decode audio file: {0}", absolutePath);
		return false;
	}

	m_SampleRate = decoder.outputSampleRate;
	m_Channels = decoder.outputChannels;
	m_BitDepth = ma_get_bytes_per_sample(decoder.outputFormat) * 8;

	ma_decoder_uninit(&decoder);

	m_Filepath = filepath;
	m_Filepath.make_preferred();

	return true;
}

bool AudioClip::Load(const std::filesystem::path& filepath, const std::vector<uint8_t>& data)
{
	PROFILE_FUNCTION();
	if (data.empty())
	{
		ENGINE_ERROR("Audio data is empty");
		return false;
	}

	ma_decoder decoder;
	if (ma_decoder_init_memory(data.data(), data.size(), NULL, &decoder) != MA_SUCCESS)
	{
		ENGINE_ERROR("Failed to decode audio data");
		return false;
	}

	m_SampleRate = decoder.outputSampleRate;
	m_Channels = decoder.outputChannels;
	m_BitDepth = ma_get_bytes_per_sample(decoder.outputFormat) * 8;
	ma_decoder_uninit(&decoder);

	m_Filepath = filepath;
	m_Filepath.make_preferred();

	return false;
}
