#include "cereal/cereal.hpp"

struct AudioListenerComponent
{
	AudioListenerComponent() = default;
	AudioListenerComponent(const AudioListenerComponent&) = default;

	bool primary = true;
private:
	friend cereal::access;
	template<typename Archive>
	void serialize(Archive& archive)
	{
		archive(primary);
	}
};
