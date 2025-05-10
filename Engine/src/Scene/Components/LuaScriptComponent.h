#pragma once

#include "cereal/cereal.hpp"
#include "sol/sol.hpp"
#include <vector>
#include <filesystem>

#include "Asset/LuaScript.h"

class Entity;
class PhysicsEngine2D;
class b2Fixture;

struct LuaScriptComponent
{
	LuaScriptComponent() = default;
	LuaScriptComponent(const std::filesystem::path& filepath);
	LuaScriptComponent(const LuaScriptComponent&) = default;

	~LuaScriptComponent();

	Ref<LuaScript> script;
	bool created = false;

	std::optional<std::pair<int, std::string>> ParseScript(Entity entity);

	void OnCreate();
	void OnDestroy();
	void OnUpdate(float deltaTime);
	void OnFixedUpdate();
	void OnDebugRender();

	void OnBeginContact(b2Fixture* fixture, Vector2f normal, Vector2f point);
	void OnEndContact(b2Fixture* fixture);

	void OnPressed();
	void OnReleased();
	void OnHovered();
	void OnUnHovered();

	bool IsContactListener();

	const std::vector<b2Fixture*>& GetFixtures() const { return m_Fixtures; }
	//const std::vector<b2Fixture*>& GetContacts() const { return m_Contacts; }

private:
	friend cereal::access;

	template<typename Archive>
	void save(Archive& archive) const
	{
		SerializationUtils::SaveAssetToArchive(archive, script);
	}

	template<typename Archive>
	void load(Archive& archive)
	{
		SerializationUtils::LoadAssetFromArchive(archive, script);
	}

	friend PhysicsEngine2D;

	std::vector<b2Fixture*> m_Fixtures;

	Ref<sol::environment> m_SolEnvironment;
	Ref<sol::protected_function> m_OnCreateFunc;
	Ref<sol::protected_function> m_OnDestroyFunc;
	Ref<sol::protected_function> m_OnUpdateFunc;
	Ref<sol::protected_function> m_OnFixedUpdateFunc;
	Ref<sol::protected_function> m_OnDebugRenderFunc;
	Ref<sol::protected_function> m_OnBeginContactFunc;
	Ref<sol::protected_function> m_OnEndContactFunc;
};