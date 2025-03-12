#include "Core/Asset.h"

class Audio : public Asset
{
public:
	Audio();
	Audio(const std::filesystem::path& filepath);
	~Audio();

	virtual bool Load(const std::filesystem::path& filepath) override;

private:
	int m_SampleRate = 0;
	int m_Channels = 0;
	int m_BitDepth = 0;
}