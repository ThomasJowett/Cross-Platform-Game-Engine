#include "cereal/cereal.hpp"
#include "Asset/AudioClip.h"
#include "Utilities/SerializationUtils.h"

struct AudioSourceComponent
{
	AudioSourceComponent() = default;
	AudioSourceComponent(const AudioSourceComponent&) = default;

	Ref<AudioClip> audioClip;

	float volume = 1.0f;
	float pitch = 1.0f;
	bool loop = false;

	bool stream = false;

private:
	friend cereal::access;
	template<typename Archive>
	void save(Archive& archive) const
	{
		archive(volume, pitch, loop, stream);
		SerializationUtils::SaveAssetToArchive(archive, audioClip);
	}

	template<typename Archive>
	void load(Archive& archive)
	{
		archive(volume, pitch, loop, stream);
		SerializationUtils::LoadAssetFromArchive(archive, audioClip);
	}
};