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

AudioClip::~AudioClip()
{
}

bool AudioClip::Load(const std::filesystem::path& filepath)
{
	PROFILE_FUNCTION();
	if (!std::filesystem::exists(filepath))
	{
		ENGINE_ERROR("Font does not exist: {0}", filepath);
		return false;
	}

	ma_decoder decoder;
	if (ma_decoder_init_file(filepath.string().c_str(), NULL, &decoder) != MA_SUCCESS)
	{
		ENGINE_ERROR("Failed to decode audio file: {0}", filepath);
		return false;
	}

	m_SampleRate = decoder.outputSampleRate;
	m_Channels = decoder.outputChannels;
	m_BitDepth = ma_get_bytes_per_sample(decoder.outputFormat) * 8;

	ma_decoder_uninit(&decoder);

	m_Filepath = filepath;

	return true;
}
