#pragma once

#include "Engine.h"

class ExampleScript : public ScriptableEntity
{
public:
	void OnCreate() override;
	void OnDestroy() override;
	void OnUpdate(float deltaTime) override;
private:
	static ScriptRegister<ExampleScript> reg;
};