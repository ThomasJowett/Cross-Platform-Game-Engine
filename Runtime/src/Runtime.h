#pragma once
#include "Core/EntryPoint.h"

class Runtime : public Application
{
public:
	explicit Runtime(const WindowProps& props);
	~Runtime() = default;
protected:
	virtual void OnUpdate() override;

private:
};

Application* CreateApplication()
{
	std::ifstream file;
	file.open(Application::GetWorkingDirectory() / "Startup", std::ios::in | std::ios::binary);
	if (!file.good())
		return new Runtime(WindowProps("Runtime", 1920, 1080, 100, 100));
	size_t size;
	std::string windowName;
	file.read((char*)&size, sizeof(size));
	windowName.resize(size);
	file.read((char*)&windowName[0], size);
	file.close();
	return new Runtime(WindowProps(windowName));
}