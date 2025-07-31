#include "BundleAudioStream.h"
#include "Scene/AssetManager.h"

BundleAudioStream::~BundleAudioStream()
{
	Uninit();
}

static size_t MZWriteToBuffer(void* pUser, mz_uint64 offset, const void* pBuf, size_t bytes)
{
	std::vector<uint8_t>* buffer = static_cast<std::vector<uint8_t>*>(pUser);
	const uint8_t* src = static_cast<const uint8_t*>(pBuf);
	if (offset + bytes > buffer->size())
	{
		buffer->resize(offset + bytes);
	}
	std::memcpy(buffer->data() + offset, src, bytes);
	return bytes;
}

ma_result BundleAudioStream::Init(const std::filesystem::path& filepath, bool stream)
{
	ma_data_source_config baseConfig = ma_data_source_config_init();
	baseConfig.vtable = &s_VTable;

	if (ma_data_source_init(&baseConfig, &m_Base) != MA_SUCCESS)
	{
		return MA_ERROR;
	}

	auto archive = AssetManager::GetBundleArchive();
	std::string zipPath = filepath.string();
	std::replace(zipPath.begin(), zipPath.end(), '\\', '/');

	if (!stream)
	{
		size_t fileSize = 0;

		void* fileData = mz_zip_reader_extract_file_to_heap(archive, zipPath.c_str(), &fileSize, 0);
		if (!fileData)
		{
			return MA_ERROR;
		}

		m_DecoderBuffer.assign(static_cast<uint8_t*>(fileData), static_cast<uint8_t*>(fileData) + fileSize);
		mz_free(fileData);
	}
	else
	{
		mz_zip_reader_extract_file_to_callback(archive, zipPath.c_str(), MZWriteToBuffer, &m_DecoderBuffer, 0);
	}

	if (ma_decoder_init_memory(m_DecoderBuffer.data(), m_DecoderBuffer.size(), NULL, &m_Decoder) != MA_SUCCESS)
	{
		return MA_ERROR;
	}

	m_Format = m_Decoder.outputFormat;
	m_Channels = m_Decoder.outputChannels;
	m_SampleRate = m_Decoder.outputSampleRate;
	m_DecoderInitialized = true;

	return MA_SUCCESS;
}

void BundleAudioStream::Uninit()
{
	if (m_DecoderInitialized)
	{
		ma_decoder_uninit(&m_Decoder);
		m_DecoderInitialized = false;
	}

	ma_data_source_uninit(&m_Base);
}

ma_result BundleAudioStream::Read(ma_data_source* pDataSource, void* pFramesOut, ma_uint64 frameCount, ma_uint64* pFramesRead)
{
	BundleAudioStream* pThis = (BundleAudioStream*)pDataSource;
	return ma_decoder_read_pcm_frames(&pThis->m_Decoder, pFramesOut, frameCount, pFramesRead);
}

ma_result BundleAudioStream::Seek(ma_data_source* pDataSource, ma_uint64 frameIndex)
{
	BundleAudioStream* pThis = (BundleAudioStream*)pDataSource;
	return ma_decoder_seek_to_pcm_frame(&pThis->m_Decoder, frameIndex);
}

ma_result BundleAudioStream::GetDataFormat(ma_data_source* pDataSource, ma_format* pFormat, ma_uint32* pChannels, ma_uint32* pSampleRate, ma_channel* pChannelMap, size_t channelMapCap)
{
	BundleAudioStream* pThis = (BundleAudioStream*)pDataSource;
	if (pFormat) {
		*pFormat = pThis->m_Format;
	}
	if (pChannels) {
		*pChannels = pThis->m_Channels;
	}
	if (pSampleRate) {
		*pSampleRate = pThis->m_SampleRate;
	}
	if (pChannelMap && channelMapCap > 0) {
		for (size_t i = 0; i < std::min(channelMapCap, static_cast<size_t>(MA_MAX_CHANNELS)); ++i) {
			pChannelMap[i] = (ma_channel)i;
		}
	}
	return MA_SUCCESS;
}

ma_result BundleAudioStream::GetCursor(ma_data_source* pDataSource, ma_uint64* pCursor)
{
	BundleAudioStream* pThis = (BundleAudioStream*)pDataSource;
	return ma_decoder_get_cursor_in_pcm_frames(&pThis->m_Decoder, pCursor);
}

ma_result BundleAudioStream::GetLength(ma_data_source* pDataSource, ma_uint64* pLength)
{
	BundleAudioStream* pThis = (BundleAudioStream*)pDataSource;
	return ma_decoder_get_length_in_pcm_frames(&pThis->m_Decoder, pLength);
}
