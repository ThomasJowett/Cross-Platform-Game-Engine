#pragma once

#include "Scene/ScriptableEntity.h"

struct NativeScriptComponent
{
	ScriptableEntity* Instance = nullptr;

	ScriptableEntity*(*InstantiateScript)();
	void (*DestroyScript)(NativeScriptComponent*);

	NativeScriptComponent() = default;
	NativeScriptComponent(const NativeScriptComponent&) = default;

	template<typename T>
	void Bind()
	{
		InstantiateScript = []() {return static_cast<ScriptableEntity*>(new T()); };
		DestroyScript = [](NativeScriptComponent* nsc) { delete nsc->Instance; nsc->Instance = nullptr; };
	}
};