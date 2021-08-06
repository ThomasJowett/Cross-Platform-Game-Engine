#include "pch.h"
#include "EngineDLL.h"

// This is an example of an exported function.
ENGINEDLL_API int fnEngineDLL(void)
{
    Scene* scene = new Scene(std::string("Test"));
    return (int)scene->GetSceneName().size();
}
