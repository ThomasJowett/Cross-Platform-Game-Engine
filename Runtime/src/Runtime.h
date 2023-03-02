#pragma once
#include "Core/EntryPoint.h"

class Runtime : public Application
{
public:
	explicit Runtime();
	~Runtime() = default;
};

Ref<Application> CreateApplication()
{
	return CreateRef<Runtime>();
}