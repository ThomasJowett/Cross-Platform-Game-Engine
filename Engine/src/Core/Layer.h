#pragma once

#include "Core/core.h"
#include "Events/Event.h"

class Layer
{
public:
	Layer(const std::string& name = "Layer");
	virtual ~Layer();

	/// <summary>
	/// Called when the layer is added to the layer stack
	/// </summary>
	virtual void OnAttach() {}

	/// <summary>
	/// Called when the layer is removed from the layer stack
	/// </summary>
	virtual void OnDetach() {}

	/// <summary>
	/// Called 100 times a second
	/// </summary>
	virtual void OnFixedUpdate() {}

	/// <summary>
	/// Called each frame
	/// </summary>
	/// <param name="deltaTime"></param>
	virtual void OnUpdate(float deltaTime) {}

	/// <summary>
	/// Called during the ImGui frame 
	/// </summary>
	virtual void OnImGuiRender() {}

	/// <summary>
	/// Called when an event is triggered
	/// </summary>
	/// <param name="event"></param>
	virtual void OnEvent(Event& event) {}

protected:
	std::string m_debugName;
};
