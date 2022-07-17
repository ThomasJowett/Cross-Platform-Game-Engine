#include "Runtime.h"
#include "Scene/SceneManager.h"
#include "Engine.h"

Runtime::Runtime(const WindowProps& props)
	:Application(props)
{
	Application::SetOpenDocument(Application::GetWorkingDirectory() / "Startup");

	std::ifstream file;
	file.open(Application::GetWorkingDirectory() / "Startup", std::ios::in | std::ios::binary);
	if (!file.good())
		return;
	size_t size;
	std::string gameName;
	std::string startupScene;
	file.read((char*)&size, sizeof(size));
	gameName.resize(size);
	file.read((char*)&gameName[0], size);
	file.read((char*)&size, sizeof(size));
	startupScene.resize(size);
	file.read((char*)&startupScene[0], size);
	file.close();

	SceneManager::ChangeScene(std::filesystem::path(startupScene), true);
}

void Runtime::OnUpdate()
{
	RenderCommand::Clear();
	SceneManager::CurrentScene()->Render(nullptr);
}