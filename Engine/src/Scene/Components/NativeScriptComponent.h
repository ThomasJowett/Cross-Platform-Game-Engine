#pragma once

#include "Scene/ScriptableEntity.h"

#include "Core/core.h"

#include "cereal/cereal.hpp"

struct NativeScriptComponent
{
	std::function<ScriptableEntity* (const std::string&)> InstantiateScript;
	std::function<void(NativeScriptComponent*)> DestroyScript;

	ScriptableEntity* Instance = nullptr;

	NativeScriptComponent() = default;
	template <typename... Args>
	NativeScriptComponent(const std::string& name, Args... args) : Name(name) { Bind(Name, args...); }
	NativeScriptComponent(const NativeScriptComponent&) = default;

	std::string Name;

	std::filesystem::path file;

	template <typename T, typename... Args>
	void Bind(Args... args)
	{
		Name = typeid(T).name();
		InstantiateScript = [args...](const std::string&)
		{ return static_cast<ScriptableEntity*>(new T(args...)); };

		DestroyScript = [](NativeScriptComponent* nsc)
		{
			delete nsc->Instance;
			nsc->Instance = nullptr;
		};
	}

	template <typename... Args>
	void Bind(const std::string& scriptName, Args... args) // TODO: get factory with argument packs working
	{
		Name = scriptName;
		InstantiateScript = [args...](const std::string& name)
		{
			ASSERT(Factory<ScriptableEntity>::Contains(name), "Could not find Native Script");
			return Factory<ScriptableEntity>::CreateInstance(name, args...);
		};

		DestroyScript = [](NativeScriptComponent* nsc)
		{
			delete nsc->Instance;
			nsc->Instance = nullptr;
		};
	}

	template <typename Archive>
	void serialize(Archive& archive)
	{
		archive(cereal::make_nvp("Script", Name));
		Bind(Name);
	}
};