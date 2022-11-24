#pragma once

#include "Utilities/Random.h"

class Uuid
{
public:
	Uuid() :m_Hi(Random::Int64()), m_Lo(Random::Int64()) {}
	Uuid(uint64_t lo, uint64_t hi) :m_Lo(lo), m_Hi(hi) {}
	Uuid(const Uuid& rhs) : m_Lo(rhs.m_Lo), m_Hi(rhs.m_Hi) {}
	Uuid(std::string string)
	{
		size_t found;
		if ((found = string.find("-")) != std::string::npos)
		{
			m_Lo = std::stoull(string.substr(0, found));
			m_Hi = std::stoull(string.substr(found + 1, std::string::npos));
		}
		else
		{
			m_Lo = std::stoull(string);
			m_Hi = Random::Int64();
		}
	}

	std::string to_string() const
	{
		return std::to_string(m_Lo) + "-" + std::to_string(m_Hi);
	}

	operator std::string() const { return to_string(); }
	bool operator==(const Uuid& rhs) const { return (m_Lo == rhs.m_Lo) && (m_Hi == rhs.m_Hi); }
	bool operator!=(const Uuid& rhs) const { return (m_Lo != rhs.m_Lo) || (m_Hi != rhs.m_Hi); }
	bool operator<(const Uuid& rhs) const
	{
		if (m_Hi == rhs.m_Hi)
			return m_Lo < rhs.m_Lo;
		else
			return m_Hi < rhs.m_Lo;
	}
private:
	uint64_t m_Lo;
	uint64_t m_Hi;
};

namespace std 
{
	template <typename T> struct hash;

	template<>
	struct hash<Uuid>
	{
		std::size_t operator()(const Uuid& uuid) const
		{
			// A perfect hash is not required
			const int round = sizeof(Uuid) / sizeof(size_t);
			const size_t* ptr = (const size_t*)&uuid;
			size_t result = ptr[0];
			for (int n = 1; n < round; ++n)
				result ^= ptr[n];
			return result;
		}
	};
}