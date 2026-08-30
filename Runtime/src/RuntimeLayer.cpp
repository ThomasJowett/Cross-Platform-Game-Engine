#include "RuntimeLayer.h"
#include "Scene/SceneManager.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/Renderer.h"
#include "Renderer/Renderer2D.h"
#include "Events/ApplicationEvent.h"

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
	Renderer2D::ResetStats();
	Renderer::RenderScene(SceneManager::CurrentScene());
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
