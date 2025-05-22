#pragma once
#include <string>
#include <vector>
#include <miniaudio/miniaudio.h>
#include <miniz.h>

class BundleAudioStream
{
public:
	BundleAudioStream() = default;
	~BundleAudioStream();

	ma_result Init(const std::filesystem::path& filepath);
	void Uninit();

	ma_data_source_base* GetDataSource() { return &m_Base; }
private:
	static ma_result Read(ma_data_source* pDataSource, void* pFramesOut, ma_uint64 frameCount, ma_uint64* pFramesRead);
	static ma_result Seek(ma_data_source* pDataSource, ma_uint64 frameIndex);
	static ma_result GetDataFormat(ma_data_source* pDataSource, ma_format* pFormat, ma_uint32* pChannels, ma_uint32* pSampleRate, ma_channel* pChannelMap, size_t channelMapCap);
	static ma_result GetCursor(ma_data_source* pDataSource, ma_uint64* pCursor);
	static ma_result GetLength(ma_data_source* pDataSource, ma_uint64* pLength);
private:
	ma_data_source_base m_Base;
	std::string filepath;

	ma_format m_Format = ma_format_unknown;
	ma_uint32 m_Channels = 0;
	ma_uint32 m_SampleRate = 0;
	std::vector<uint8_t> m_DecoderBuffer;
	ma_decoder m_Decoder;
	bool m_DecoderInitialized = false;

	static inline ma_data_source_vtable s_VTable = {
		Read,
		Seek,
		GetDataFormat,
		GetCursor,
		GetLength
	};
};