#pragma once

#include "Scene/ScriptableEntity.h"

struct NativeScriptComponent
{
	ScriptableEntity* Instance = nullptr;

	std::function<void()> InstantiateFunction;
	std::function<void()> DestroyInstanceFunction;

	std::function<void(ScriptableEntity*)>OnCreateFunction;
	std::function<void(ScriptableEntity*)>OnDestroyFunction;
	std::function<void(ScriptableEntity*, float)>OnUpdateFunction;

	template<typename T>
	void Bind()
	{
		InstantiateFunction = [&]() {Instance = new T(); };
		DestroyInstanceFunction = [&]() { delete (T*)Instance; Instance = nullptr; };

		OnCreateFunction = [](ScriptableEntity* instance) {((T*)instance)->OnCreate(); };
		OnDestroyFunction = [](ScriptableEntity* instance) {((T*)instance)->OnDestroy(); };
		OnUpdateFunction = [](ScriptableEntity* instance, float deltaTime) {((T*)instance)->OnUpdate(deltaTime); };
	}
};