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
		return (float)s_UniformDistribution(s_RandomEngine) / (float)(std::numeric_limits<std::mt19937_64::result_type>::max)();
	}

	// random float in range with uniform distribution
	static float FloatInRange(float min = 0.0f, float max = 1.0f)
	{
		return min + (Float() * (max - min));
	}

	// random float using gaussian normal distribution
	static float NormalDistribution(float mean = 0.0f, float sigma = 1.0f)
	{
		return (float)(s_NormalDistribution(s_RandomEngine) * sigma) + mean;
	}

	// random 64 bit integer
	static uint64_t Int64()
	{
		return s_UniformDistribution(s_RandomEngine);
	}

	// random bool
	static bool Bool()
	{
		return std::uniform_int_distribution<>{0,1}(s_RandomEngine);
	}

private:
	inline static std::mt19937_64 s_RandomEngine;
	inline static std::uniform_int_distribution<std::mt19937_64::result_type> s_UniformDistribution;
	inline static std::normal_distribution<double> s_NormalDistribution;
};