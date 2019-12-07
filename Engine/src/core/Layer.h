#pragma once

#include "Core/core.h"
#include "Events/Event.h"

class Layer
{
public:
	Layer(const std::string& name = "Layer");
	virtual ~Layer();

	virtual void OnAttach() {}
	virtual void OnDetach() {}
	virtual void OnUpdate(float deltaTime) {}
	virtual void OnImGuiRender() {}
	virtual void OnEvent(Event& event) {}

	inline const std::string& GetName() const { return m_debugName; }

protected:
	std::string m_debugName;
};
