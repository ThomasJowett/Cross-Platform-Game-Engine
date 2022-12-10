#pragma once
#include "Core/EntryPoint.h"

class Editor : public Application
{
public:
	explicit Editor(const WindowProps& props);
	~Editor() = default;
};

/*Entry point*/
Ref<Application> CreateApplication()
{
	return CreateRef<Editor>(WindowProps("Editor", 1920, 1080, 100, 100));
}