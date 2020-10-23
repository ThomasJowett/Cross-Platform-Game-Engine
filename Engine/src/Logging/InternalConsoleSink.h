#pragma once
#include "spdlog/sinks/base_sink.h"
#include <mutex>
#include <vector>

template<class Mutex>
class InternalConsoleSink : public spdlog::sinks::base_sink<std::mutex>
{
public:
	using Message = std::pair<std::string, spdlog::level::level_enum>;

	explicit InternalConsoleSink()
	{
	}

	InternalConsoleSink(const InternalConsoleSink&) = delete;
	InternalConsoleSink& operator=(const InternalConsoleSink&) = delete;
	virtual ~InternalConsoleSink() = default;

public:
	static uint16_t s_MessageBufferCapacity;
	static uint16_t s_MessageBufferSize;
	static uint16_t s_MessageBufferBegin;
	static std::vector<Message> s_MessageBuffer;
protected:
	void sink_it_(const spdlog::details::log_msg& msg) override
	{
		spdlog::memory_buf_t formatted;
		base_sink<Mutex>::formatter_->format(msg, formatted);

		AddMessage(fmt::to_string(formatted), msg.level);
	}

	void flush_() override
	{
	}

private:

	void AddMessage(const std::string& message, spdlog::level::level_enum level)
	{
		*(s_MessageBuffer.begin() + s_MessageBufferBegin) = std::make_pair(message, level);
		if (++s_MessageBufferBegin == s_MessageBufferCapacity)
			s_MessageBufferBegin = 0;
		if (s_MessageBufferSize < s_MessageBufferCapacity)
			s_MessageBufferSize++;
	}
private:

};

using InternalConsoleSink_mt = InternalConsoleSink<std::mutex>;

