#include "ExampleNativeScript.h"

ScriptRegister<ExampleScript> ExampleScript::reg("Example Script");

void ExampleScript::OnCreate()
{
}

void ExampleScript::OnDestroy()
{
}

void ExampleScript::OnUpdate(float deltaTime)
{
	CLIENT_DEBUG("This is an example script");
}
