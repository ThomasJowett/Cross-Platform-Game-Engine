#pragma once
#include <iostream>
#include "Debug.h"

#include <chrono>
template<typename Fn>
class Timer
{
public:
	Timer(const char* name, Fn&& func, bool stopped = false)
		:m_Name(name), m_Func(func), m_Stopped(stopped)
	{
		m_startTimepoint = std::chrono::high_resolution_clock::now();
	}
	~Timer()
	{
		if(!m_Stopped)
			Stop();
	}

	void Stop()
	{
		m_Stopped = true;

		auto endTimepoint = std::chrono::high_resolution_clock::now();

		auto start = std::chrono::time_point_cast<std::chrono::microseconds>(m_startTimepoint).time_since_epoch().count();
		auto end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

		auto duration = end - start;

		double ms = duration * 0.001;

		m_Func({ m_Name, ms });
	}


private:
	const char* m_Name;
	std::chrono::time_point<std::chrono::high_resolution_clock> m_startTimepoint;
	bool m_Stopped;

	Fn m_Func;
};