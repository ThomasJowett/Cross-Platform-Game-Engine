#pragma once

#include "Core/core.h"
#include "Events/Event.h"

class DLLIMPEXP_CORE Layer
{
public:
	Layer(const std::string& name = "Layer");
	virtual ~Layer();

	virtual void OnAttach() {}
	virtual void OnDetach() {}
	virtual void OnUpdate() {}
	virtual void OnEvent(Event& event) {}

	inline const std::string& GetName() const { return m_debugName; }

protected:
	std::string m_debugName;
};
