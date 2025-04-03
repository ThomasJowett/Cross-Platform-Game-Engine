#include "cereal/cereal.hpp"
#include "Asset/AudioClip.h"
#include "Utilities/SerializationUtils.h"

struct ma_sound;

struct AudioSourceComponent
{
	AudioSourceComponent() = default;
	AudioSourceComponent(const AudioSourceComponent&) = default;

	Ref<AudioClip> audioClip;

	float volume = 1.0f;
	float pitch = 1.0f;
	bool loop = false;

	float minDistance = 1.0f;
	float maxDistance = 10.0f;

	float rolloff = 1.0f;

	bool stream = false;

	bool playOnStart = false;

	Ref<ma_sound> sound;

	void Play() { play = true; }
	void Pause() { pause = true; }
	void Stop() { stop = true; }

private:
	friend class Scene;
	bool play = false;
	bool pause = false;
	bool stop = false;
	friend cereal::access;
	template<typename Archive>
	void save(Archive& archive) const
	{
		archive(volume, pitch, loop, minDistance, maxDistance, rolloff, stream, playOnStart);
		SerializationUtils::SaveAssetToArchive(archive, audioClip);
	}

	template<typename Archive>
	void load(Archive& archive)
	{
		archive(volume, pitch, loop, minDistance, maxDistance, rolloff, stream, playOnStart);
		SerializationUtils::LoadAssetFromArchive(archive, audioClip);
	}
};