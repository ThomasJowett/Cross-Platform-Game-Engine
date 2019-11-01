#pragma once

namespace Debug
{
#define DBG_OUTPUT(...) Debug::Output(__FILE__, __LINE__, __VA_ARGS__)
#define OUTPUT(...) Debug::Output(__VA_ARGS__)

#ifdef JPT_PLATFORM_WINDOWS
	// wide char output
	static void Output(const WCHAR* pFormat, ...)
	{
		WCHAR buffer[1024] = { 0 };
		va_list args;
		va_start(args, pFormat);
		wvsprintf(buffer, pFormat, args);
		va_end(args);
	
		OutputDebugString(buffer);
	}

	// char output
	static void Output(const CHAR* pFormat, ...)
	{
		size_t length = strlen(pFormat);

		WCHAR format[1024];

		mbstowcs_s(&length,format, pFormat, length);

		WCHAR buffer[1024] = { 0 };
		va_list args;
		va_start(args, pFormat);
		wvsprintf(buffer, format, args);
		va_end(args);
	
		OutputDebugString(buffer);
	}

	// wide char output with file and line number
	static void Output(const char* file, const int line, const WCHAR *pFormat, ...)
	{
		WCHAR buffer[1024] = { 0 };
		int stringLength = wsprintf(buffer, L"%hs(%d): ", file, line);

		va_list args;
		va_start(args, pFormat);
		wvsprintf(buffer + stringLength, pFormat, args);
		va_end(args);

		OutputDebugString(buffer);
	}

	// char output with file and line number
	static void Output(const char* file, const int line, const CHAR* pFormat, ...)
	{
		size_t length = strlen(pFormat);
		WCHAR format[1024];
		mbstowcs_s(&length, format, pFormat, length);
		
		WCHAR buffer[1024] = { 0 };
		int stringLength = wsprintf(buffer, L"%hs(%d): ", file, line);
	
		va_list args;
		va_start(args, pFormat);
		wvsprintf(buffer + stringLength, format, args);
		va_end(args);
	
		OutputDebugString(buffer);
	}

#else
#error Target platform not supported
#endif



#define HR(x)																\
	{																		\
		HRESULT hr = (x);													\
		if (FAILED(hr))														\
		{																	\
			Debug::Output(__FILE__, (DWORD)__LINE__, L"HRESULT ERROR\n");	\
		}																	\
	}
};