#pragma once

#include <unordered_map>

class Uuid
{
public:
	Uuid();
	Uuid(uint64_t uuid);
	Uuid(const Uuid&) = default;

	operator uint64_t() const { return m_UUID; }
private:
	uint64_t m_UUID;
};

namespace std 
{
	template<>
	struct hash<Uuid>
	{
		std::size_t operator()(const Uuid& uuid) const
		{
			return hash<uint64_t>()((uint64_t)uuid);
		}
	};
}