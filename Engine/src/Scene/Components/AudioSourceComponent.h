#include "cereal/cereal.hpp"
#include "Asset/AudioClip.h"
#include "Utilities/SerializationUtils.h"
#include "Utilities/BundleAudioStream.h"
#include "Scripting/Lua/LuaBindings.h"

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
	Ref<BundleAudioStream> bundleStream;

	void Play() { play = true; }
	void Pause() { pause = true; }
	void Stop() { stop = true; }

	REFLECT_LUA_BEGIN(AudioSourceComponent)
		REFLECT_LUA_PROPERTY_CUSTOM("Clip", "The audio clip to play", "AudioClip",
			([](Self& c) { return c.audioClip; }),
			([](Self& c, const Ref<AudioClip>& v) { c.audioClip = v; }))
		REFLECT_LUA_PROPERTY_CUSTOM("Volume", "Volume multiplier, 1 being the clip's own volume", "number",
			([](Self& c) { return c.volume; }),
			([](Self& c, float v) { c.volume = v; }))
		REFLECT_LUA_PROPERTY_CUSTOM("Pitch", "Playback speed and pitch multiplier", "number",
			([](Self& c) { return c.pitch; }),
			([](Self& c, float v) { c.pitch = v; }))
		REFLECT_LUA_PROPERTY_CUSTOM("Loop", "Restart the clip when it finishes", "boolean",
			([](Self& c) { return c.loop; }),
			([](Self& c, bool v) { c.loop = v; }))
		REFLECT_LUA_PROPERTY_CUSTOM("MinDistance", "Distance from the listener within which the clip plays at full volume", "number",
			([](Self& c) { return c.minDistance; }),
			([](Self& c, float v) { c.minDistance = v; }))
		REFLECT_LUA_PROPERTY_CUSTOM("MaxDistance", "Distance from the listener beyond which the clip stops getting quieter", "number",
			([](Self& c) { return c.maxDistance; }),
			([](Self& c, float v) { c.maxDistance = v; }))
		REFLECT_LUA_PROPERTY_CUSTOM("Rolloff", "How quickly the volume falls off between MinDistance and MaxDistance", "number",
			([](Self& c) { return c.rolloff; }),
			([](Self& c, float v) { c.rolloff = v; }))
		REFLECT_LUA_PROPERTY_CUSTOM("Stream", "Stream the clip from disk instead of decoding it all up front", "boolean",
			([](Self& c) { return c.stream; }),
			([](Self& c, bool v) { c.stream = v; }))
		REFLECT_LUA_FUNCTION(Play, "Start or resume playback")
		REFLECT_LUA_FUNCTION(Pause, "Pause playback")
		REFLECT_LUA_FUNCTION(Stop, "Stop playback and rewind to the start")
	REFLECT_LUA_END()

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