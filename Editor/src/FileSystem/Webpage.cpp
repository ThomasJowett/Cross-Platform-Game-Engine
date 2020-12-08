#include "Webpage.h"

#ifdef __WINDOWS__
#include <Windows.h>	
#include <shellapi.h>
void Webpage::OpenWebpage(const std::wstring& url)
{
	ShellExecute(0, 0, url.c_str(), 0, 0, SW_SHOW);
}
#endif // __WINDOWS__

#ifdef __WINDOWS__
#elif __linux__
void Webpage::OpenWebpage(const std::wstring& url)
{
	std::string command = "xdg-open " + std::string(url.begin(), url.end());
	
	system(command.c_str());
}
#elif __APPLE__
void Webpage::OpenWebpage(const std::wstring& url)
{
	std::string command = "open " + std::string(url.begin(), url.end());

	system(command.c_str());
}
#endif