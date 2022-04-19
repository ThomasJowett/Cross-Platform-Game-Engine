#pragma once
#include "Core/EntryPoint.h"

class Runtime : public Application
{
public:
    explicit Runtime(const WindowProps& props);
    ~Runtime() = default;
};

Application* CreateApplication()
{
    return new Runtime(WindowProps("Runtime", 1920, 1080, 100, 100));
}