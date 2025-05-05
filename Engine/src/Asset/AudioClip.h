#include "Core/Asset.h"

class AudioClip : public Asset
{
public:
	AudioClip();
	AudioClip(const std::filesystem::path& filepath);
	AudioClip(const std::filesystem::path& filepath, const std::vector<uint8_t>& data);
	~AudioClip();

	virtual bool Load(const std::filesystem::path& filepath) override;
	virtual bool Load(const std::filesystem::path& filepath, const std::vector<uint8_t>& data) override;

	int GetSampleRate() const { return m_SampleRate; }
	int GetChannels() const { return m_Channels; }
	int GetBitDepth() const { return m_BitDepth; }

private:
	int m_SampleRate = 0;
	int m_Channels = 0;
	int m_BitDepth = 0;
};