#pragma once

#include "Scene/ScriptableEntity.h"

#include "Core/core.h"

#include "cereal/cereal.hpp"

struct NativeScriptComponent
{
	ScriptableEntity* Instance = nullptr;

	ScriptableEntity*(*InstantiateScript)(const std::string&) = nullptr;
	void (*DestroyScript)(NativeScriptComponent*) = nullptr;

	NativeScriptComponent() = default;
	NativeScriptComponent(const std::string& name):Name(name) {Bind(Name); }
	NativeScriptComponent(const NativeScriptComponent&) = default;

	std::string Name;

	template<typename T>
	void Bind()
	{
		Name = typeid(T).name();
		InstantiateScript = [](const std::string&) {return static_cast<ScriptableEntity*>( new T()); };
		DestroyScript = [](NativeScriptComponent* nsc) { delete nsc->Instance; nsc->Instance = nullptr; };
	}

	void Bind(const std::string& scriptName)
	{
		Name = scriptName;
		InstantiateScript = [](const std::string& name) {return static_cast<ScriptableEntity*>(Factory<ScriptableEntity>::CreateInstance(name)); };
		DestroyScript = [](NativeScriptComponent* nsc) { delete nsc->Instance; nsc->Instance = nullptr; };
	}

	template<typename Archive>
	void serialize(Archive& archive)
	{
		archive(cereal::make_nvp("Script", Name));
		Bind(Name);
	}
};