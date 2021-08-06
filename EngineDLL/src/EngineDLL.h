#ifdef ENGINEDLL_EXPORTS
#define ENGINEDLL_API extern "C" __declspec(dllexport)
#else
#define ENGINEDLL_API extern "C" __declspec(dllimport)
#endif


ENGINEDLL_API int fnEngineDLL(void);
