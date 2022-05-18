#pragma once

#include <string>
#include <chrono>
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <mutex>
#include <thread>

#include "Logger.h"
#include "Core/Application.h"

using FloatingPointMicroSeconds = std::chrono::duration<double, std::micro>;

struct ProfileResult
{
	std::string name;
	FloatingPointMicroSeconds start;
	std::chrono::microseconds elapsedTime;
	std::thread::id threadID;
};

struct InstrumentationSession
{
	std::string name;
};

class Instrumentor
{
private:
	std::mutex m_Mutex;
	InstrumentationSession* m_CurrentSession;
	std::ofstream m_OutputStream;

	bool m_IsEnabled = false;
public:
	Instrumentor()
		: m_CurrentSession(nullptr)
	{
	}

	void BeginSession(const std::string& name, const std::string& filepath = "results.json")
	{
		if (!m_IsEnabled)
			return;

		std::lock_guard lock(m_Mutex);
		if (m_CurrentSession)
		{
			ENGINE_WARN("Beginning new session before last has ended. Force ending last session...");
			// If there is already a current session, then close it before beginning a new one.
			// Subsequent profiling output meant for the original session will end up in the new one instead.
			InternalEndSession(m_CurrentSession->name);
		}

		m_OutputStream.open(Application::GetWorkingDirectory() / filepath);

		if (m_OutputStream.is_open())
		{
			m_CurrentSession = new InstrumentationSession({ name });
			WriteHeader();
		}
		else
		{
			ENGINE_ERROR("Instrumentor could not open Results file: {0}", filepath);
		}
	}

	void EndSession(const std::string& name)
	{
		if (!m_IsEnabled)
			return;
		std::lock_guard lock(m_Mutex);
		InternalEndSession(name);
	}

	void WriteProfile(const ProfileResult& result)
	{
		if (!m_IsEnabled)
			return;

		std::stringstream json;

		std::string name = result.name;
		std::replace(name.begin(), name.end(), '"', '\'');

		json << std::setprecision(3) << std::fixed;
		json << ",{";
		json << "\"cat\":\"function\",";
		json << "\"dur\":" << (result.elapsedTime.count()) << ',';
		json << "\"name\":\"" << name << "\",";
		json << "\"ph\":\"X\",";
		json << "\"pid\":0,";
		json << "\"tid\":" << result.threadID << ",";
		json << "\"ts\":" << result.start.count();
		json << "}";

		std::lock_guard lock(m_Mutex);
		if (m_CurrentSession)
		{
			m_OutputStream << json.str();
			m_OutputStream.flush();
		}
	}

	static Instrumentor& Get()
	{
		static Instrumentor instance;
		return instance;
	}

	static void Enable()
	{
		Get().EnableImpl();
	}

	static void Disable()
	{
		Get().DisableImpl();
	}

	static bool IsEnabled()
	{
		return Get().IsEnabledImpl();
	}

private:
	void WriteHeader()
	{
		m_OutputStream << "{\"otherData\": {},\"traceEvents\":[{}";
		m_OutputStream.flush();
	}

	void WriteFooter()
	{
		m_OutputStream << "]}";
		m_OutputStream.flush();
	}

	void InternalEndSession(const std::string& name)
	{
		if (m_CurrentSession)
		{
			if (m_CurrentSession->name != name)
			{
				ENGINE_WARN("Attempting to end session \"{0}\" but does not match current session \"{1}\"", name, m_CurrentSession->name);
			}
			WriteFooter();
			m_OutputStream.close();
			delete m_CurrentSession;
			m_CurrentSession = nullptr;
		}
	}

	void EnableImpl()
	{
		m_IsEnabled = true;
	}

	void DisableImpl()
	{
		m_IsEnabled = false;
	}

	bool IsEnabledImpl()
	{
		return m_IsEnabled;
	}
};

class InstrumentationTimer
{
public:
	InstrumentationTimer(const char* name)
		: m_Name(name), m_Stopped(false)
	{
		if (Instrumentor::IsEnabled())
		{
			m_StartTimepoint = std::chrono::steady_clock::now();
		}
	}

	~InstrumentationTimer()
	{
		if (Instrumentor::IsEnabled())
		{
			if (!m_Stopped)
				Stop();
		}
	}

	void Stop()
	{
		auto endTimePoint = std::chrono::steady_clock::now();
		auto highResStart = FloatingPointMicroSeconds{ m_StartTimepoint.time_since_epoch() };
		auto elapsedTime = std::chrono::time_point_cast<std::chrono::microseconds>(endTimePoint).time_since_epoch()
			- std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch();

		Instrumentor::Get().WriteProfile({ m_Name, highResStart, elapsedTime, std::this_thread::get_id() });

		m_Stopped = true;
	}
private:
	const char* m_Name;
	std::chrono::time_point<std::chrono::steady_clock> m_StartTimepoint;
	bool m_Stopped;
};

#ifndef DEBUG // You should only profile an application in release
	// Resolve which function signature macro will be used. Note that this only
	// is resolved when the (pre)compiler starts, so the syntax highlighting
	// could mark the wrong one in your editor!
#if defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000)) || (defined(__ICC) && (__ICC >= 600)) || defined(__ghs__)
	#define FUNC_SIG __PRETTY_FUNCTION__
#elif defined(__DMC__) && (__DMC__ >= 0x810)
	#define FUNC_SIG __PRETTY_FUNCTION__
#elif defined(__FUNCSIG__)
	#define FUNC_SIG __FUNCSIG__
#elif (defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 600)) || (defined(__IBMCPP__) && (__IBMCPP__ >= 500))
	#define FUNC_SIG __FUNCTION__
#elif defined(__BORLANDC__) && (__BORLANDC__ >= 0x550)
	#define FUNC_SIG __FUNC__
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
	#define FUNC_SIG __func__
#elif defined(__cplusplus) && (__cplusplus >= 201103)
	#define FUNC_SIG __func__
#else
	#define FUNC_SIG "FUNC_SIG unknown!"
#endif

#define PROFILE_BEGIN_SESSION(name, filepath) ::Instrumentor::Get().BeginSession(name, filepath)
#define PROFILE_END_SESSION(name) ::Instrumentor::Get().EndSession(name)
#define PROFILE_SCOPE(name) ::InstrumentationTimer timer##__LINE__(name);
#define PROFILE_FUNCTION() PROFILE_SCOPE(FUNC_SIG)
#else
#define PROFILE_BEGIN_SESSION(name, filepath)
#define PROFILE_END_SESSION(name)
#define PROFILE_FUNCTION()
#define PROFILE_SCOPE(name)
#endif // PROFILE