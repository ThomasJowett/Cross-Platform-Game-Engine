#pragma once

#include "Core/Layer.h"

class RuntimeLayer :
	public Layer
{
public:
	RuntimeLayer();

	virtual void OnAttach() override;
	virtual void OnUpdate(float deltaTime) override;
	virtual void OnEvent(Event& event) override;
};
