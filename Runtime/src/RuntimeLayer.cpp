#include "RuntimeLayer.h"
#include "Engine.h"

RuntimeLayer::RuntimeLayer()
{

}

void RuntimeLayer::OnAttach()
{
	SceneManager::ChangeSceneState(SceneState::Play);
}

void RuntimeLayer::OnUpdate(float deltaTime)
{
	RenderCommand::Clear();
	SceneManager::CurrentScene()->Render(nullptr);
}

void RuntimeLayer::OnEvent(Event& event)
{
	EventDispatcher dispatcher(event);
	dispatcher.Dispatch<WindowResizeEvent>([](WindowResizeEvent& e)
		{
			SceneManager::CurrentScene()->OnViewportResize(e.GetWidth(), e.GetHeight());
			return false;
		});
}
