#include "Webpage.h"

#ifdef _WINDOWS
#include <Windows.h>	
#include <shellapi.h>
void Webpage::OpenWebpage(const std::wstring& url)
{
	ShellExecute(0, 0, url.c_str(), 0, 0, SW_SHOW);
}
#endif // _WINDOWS

#ifdef _WINDOWS
#elif __linux__
void Webpage::OpenWebpage(const std::wstring& url)
{
	std::string command = "xdg-open " + std::string(url.begin(), url.end());

	int result = system(command.c_str());
}
#elif __APPLE__
void Webpage::OpenWebpage(const std::wstring& url)
{
	std::string command = "open " + std::string(url.begin(), url.end());

	system(command.c_str());
}
#elif defined(__EMSCRIPTEN__)
#include <emscripten.h>
void Webpage::OpenWebpage(const std::wstring& url)
{
	std::string urlString(url.begin(), url.end());
	emscripten_run_script(("window.open('" + urlString + "', '_blank');").c_str());
}
#endif