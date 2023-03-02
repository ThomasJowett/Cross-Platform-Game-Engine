#include "Runtime.h"
#include "Scene/SceneManager.h"
#include "RuntimeLayer.h"
#include "Engine.h"

Runtime::Runtime()
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

	if (!Application::CreateDesktopWindow(WindowProps(gameName, 1920, 1080, 100, 100)))
	{
		return;
	}

	SceneManager::ChangeScene(std::filesystem::path(startupScene));

	m_LayerStack.PushLayer(CreateRef<RuntimeLayer>());
}