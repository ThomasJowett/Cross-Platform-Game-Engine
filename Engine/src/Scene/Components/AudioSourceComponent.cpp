#include "cereal/cereal.hpp"
#include "Asset/Audio.h"
#include "Utilities/SerializationUtils.h"

struct AudioSourceComponent
{
	Ref<Audio> audio;
private:
	friend cereal::access;
	template<typename Archive>
	void save(Archive& archive) const
	{
		SerializationUtils::SaveAssetToArchive(archive, audio);
	}

	template<typename Archive>
	void load(Archive& archive)
	{
		SerializationUtils::LoadAssetFromArchive(archive, audio);
	}
};