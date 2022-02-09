#pragma once

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
	template <typename T> struct hash;

	template<>
	struct hash<Uuid>
	{
		std::size_t operator()(const Uuid& uuid) const
		{
			return (uint64_t)uuid;
		}
	};
}