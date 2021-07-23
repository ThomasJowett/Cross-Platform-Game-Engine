#pragma once

#include "Scene/ScriptableEntity.h"

#include "Core/core.h"

#include "cereal/cereal.hpp"

struct NativeScriptComponent
{
	std::function<ScriptableEntity*(const std::string&)> InstantiateScript;
	std::function<void(NativeScriptComponent*)> DestroyScript;

	ScriptableEntity* Instance = nullptr;

	NativeScriptComponent() = default;
	NativeScriptComponent(const std::string& name):Name(name) {Bind(Name); }
	NativeScriptComponent(const NativeScriptComponent&) = default;

	std::string Name;

	std::filesystem::path file;

	template<typename T, typename... Args>
	void Bind(Args... args)
	{
		Name = typeid(T).name();
		InstantiateScript = [args...](const std::string&) {return static_cast<ScriptableEntity*>(new T(args...)); };
		DestroyScript = [](NativeScriptComponent* nsc) { delete nsc->Instance; nsc->Instance = nullptr; };
	}

	void Bind(const std::string& scriptName)// TODO: get factory with argument packs working
	{
		Name = scriptName;
		InstantiateScript = [](const std::string& name) {return (Factory<ScriptableEntity>::CreateInstance(name)); };
		DestroyScript = [](NativeScriptComponent* nsc) { delete nsc->Instance; nsc->Instance = nullptr; };
	}

	template<typename Archive>
	void serialize(Archive& archive)
	{
		archive(cereal::make_nvp("Script", Name));
		Bind(Name);
	}
};