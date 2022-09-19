#pragma once
#include "Core/EntryPoint.h"

class Editor : public Application
{
public:
	explicit Editor(const WindowProps& props);
	~Editor() = default;

protected:
	virtual void OnUpdate();
};

/*Entry point*/
Application* CreateApplication()
{
	return new Editor(WindowProps("Editor", 1920, 1080, 100, 100));
}