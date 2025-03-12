#include "Audio.h"

#include "miniaudio/miniaudio.h"

Audio::Audio()
{
}

Audio::Audio(const std::filesystem::path& filepath)
{
}

Audio::~Audio()
{
}

bool Audio::Load(const std::filesystem::path& filepath)
{
	PROFILE_FUNCTION();
	if (!std::filesystem::exists(filepath))
	{
		ENGINE_ERROR("Font does not exist: {0}", filepath);
		return false;
	}

	ma_decoder decoder;

	ma_result result = ma_decoder_init_file(filepath.string().c_str()), nullptr, & ma_decoder);
	if (result != MA_SUCCESS)
	{
		BM_ENGINE_ERROR("Failed to decode audio file: {0}", filepath);
		return false;
	}

	m_Filepath = filepath;

	m_SampleRate = decoder.outputSampleRate;
	m_Channels = decoder.outputChannels;
	m_BitDepth = ma_get_bytes_per_sample(decoder.outputFormat) * 8;

	ma_decoder_uninit(&decoder);

	return false;
}
