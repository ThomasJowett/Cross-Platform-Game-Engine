#include "stdafx.h"
#include "UUID.h"

#include "Utilities/Random.h"

Uuid::Uuid()
	:m_UUID(Random::Int64())
{
}

Uuid::Uuid(uint64_t uuid)
	:m_UUID(uuid)
{
}
