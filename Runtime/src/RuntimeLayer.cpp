#include "RuntimeLayer.h"
#include "Scene/SceneManager.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/Renderer.h"
#include "Renderer/Renderer2D.h"
#include "Events/ApplicationEvent.h"
#include "Core/Application.h"
#include "Core/Input.h"

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

	if (SceneManager::GetSceneState() == SceneState::Play)
	{
		// The Runtime window is the game canvas 1:1 - unlike the Editor's viewport sub-panel, no
		// coordinate adjustment is needed.
		auto [mouseX, mouseY] = Input::GetMousePos();
		SceneManager::CurrentScene()->UpdateUIInput(Vector2f((float)mouseX, (float)mouseY),
			Application::GetWindow()->GetWidth(), Application::GetWindow()->GetHeight());
	}
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
