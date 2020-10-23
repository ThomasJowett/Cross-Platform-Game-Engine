#include "stdafx.h"
#include "InternalConsoleSink.h"

uint16_t InternalConsoleSink_mt::s_MessageBufferCapacity = sizeof(long long);
uint16_t InternalConsoleSink_mt::s_MessageBufferSize = 0;
uint16_t InternalConsoleSink_mt::s_MessageBufferBegin = 0;
std::vector<std::pair<std::string, spdlog::level::level_enum>> InternalConsoleSink_mt::s_MessageBuffer(s_MessageBufferCapacity);