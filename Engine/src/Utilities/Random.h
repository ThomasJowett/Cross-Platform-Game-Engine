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
		return (float)s_UniformDistribution(s_RandomEngine) / (float)(std::numeric_limits<uint32_t>::max)();
	}

	// random float in range with uniform distribution
	static float FloatInRange(float min, float max)
	{
		return min + (Float() * (max - min));
	}

	// random float using gaussian normal distribution
	static float NormalDistribution(float mean, float sigma)
	{
		return (float)(s_NormalDistribution(s_RandomEngine) * sigma) + mean;
	}

private:
	inline static std::mt19937 s_RandomEngine;
	inline static std::uniform_int_distribution<std::mt19937::result_type> s_UniformDistribution;
	inline static std::normal_distribution<double> s_NormalDistribution;
};