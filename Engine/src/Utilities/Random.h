#pragma once

#include <random>

class Random
{
public:
	static void Init()
	{
		s_RandomEngine.seed(std::random_device()());
	}

	// random float between 0 and 1
	static float Float()
	{
		return (float)s_Distribution(s_RandomEngine) / (float)std::numeric_limits<uint32_t>::max();
	}

	// random float in range
	static float FloatInRange(float min, float max)
	{
		return (Float() * max) + min;
	}

private:
	inline static std::mt19937 s_RandomEngine;
	inline static std::uniform_int_distribution<std::mt19937::result_type> s_Distribution;
};