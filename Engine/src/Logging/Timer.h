#pragma once
#include <chrono>

template<typename Fn>
class Timer
{
public:
	Timer(const char* name, Fn&& func, bool stopped = false)
		:m_Name(name), m_Func(func), m_Stopped(stopped)
	{
		if (!m_Stopped)
			Start();
	}
	~Timer()
	{
		if (!m_Stopped)
			Stop();
	}

	void Start()
	{
		m_Stopped = false;
		m_startTimepoint = std::chrono::high_resolution_clock::now();
	}

	void Stop()
	{
		m_Stopped = true;

		auto endTimepoint = std::chrono::high_resolution_clock::now();

		auto start = std::chrono::time_point_cast<std::chrono::microseconds>(m_startTimepoint).time_since_epoch().count();
		auto end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

		auto duration = end - start;

		double ms = duration * 0.001;

		m_Func(m_Name, ms);
	}

private:
	const char* m_Name;
	std::chrono::time_point<std::chrono::high_resolution_clock> m_startTimepoint;
	bool m_Stopped;

	Fn m_Func;
};

class LogTimer
{
public:
	explicit LogTimer(const char* name, bool stopped = false)
		:m_Timer(name, PrintTime, stopped) {}

	LogTimer(const LogTimer&) = delete;

	void Start() { m_Timer.Start(); }
	void Stop() { m_Timer.Stop(); }
private:
	Timer<std::function<void(const char*, double)>> m_Timer;

	static void PrintTime(const char* name, double time)
	{
		const char* unit = "ms";

		if (time > 100.0)
		{
			unit = "s";
			time /= 100.0;
		}

		ENGINE_INFO("Timer:{0} took {1}{2}", name, time, unit);
	}
};
