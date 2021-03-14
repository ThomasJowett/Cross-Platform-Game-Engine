#pragma once

#include "Core/core.h"
#include "Events/Event.h"

class Layer
{
public:
	Layer(const std::string& name = "Layer");
	virtual ~Layer();

	/**
	 * Called when the layer is added to the layer stack
	 */
	virtual void OnAttach() {}

	/**
	 * Called when the layer is removed from the layer stack
	 */
	virtual void OnDetach() {}

	/**
	 * Called 100 times a second
	 */
	virtual void OnFixedUpdate() {}

	/**
	 * Called each frame
	 * @param deltaTime 
	 */
	virtual void OnUpdate(float deltaTime) {}

	/**
	 * Called during the ImGui frame
	 */
	virtual void OnImGuiRender() {}

	/**
	 * Called when an event is triggered
	 * @param event 
	 */
	virtual void OnEvent(Event& event) {}

protected:
	std::string m_debugName;
};
