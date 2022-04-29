#include "Runtime.h"
#include "Scene/SceneManager.h"
#include "Engine.h"

//#include "MainLayer.h"

Runtime::Runtime(const WindowProps& props)
    :Application(props)
{
	std::ifstream file;
	file.open(Application::GetWorkingDirectory() / "Startup", std::ios::in | std::ios::binary);
	if (!file.good())
		return;
	size_t size;
	std::string gameName;
	std::string defaultScene;
	file.read((char*)&size, sizeof(size));
	gameName.resize(size);
	file.read((char*)&gameName[0], size);
	file.read((char*)&size, sizeof(size));
	gameName.resize(size);
	file.read((char*)&defaultScene[0], size);
	file.close();

	SceneManager::ChangeScene(std::filesystem::path(defaultScene), true);
	//PushLayer(new MainLayer());
}

void Runtime::OnUpdate()
{
	RenderCommand::Clear();
	SceneManager::CurrentScene()->Render(nullptr);
}
