#include "stdafx.h"
#include "LuaBindings.h"

#include "Logging/Instrumentor.h"
#include "LuaManager.h"
#include "Scene/SceneManager.h"
#include "Physics/HitResult2D.h"
#include "Scene/AssetManager.h"
#include "Asset/StaticMesh.h"
#include "Asset/Tileset.h"
#include "Asset/PhysicsMaterial.h"

namespace Lua
{
void ChangeScene(const std::string_view sceneFilepath)
{
	SceneManager::ChangeScene(std::filesystem::path(sceneFilepath));
}

Ref<Scene> LoadScene(const std::string_view sceneFilepath)
{
	Ref<Scene> newScene = CreateRef<Scene>(Application::GetOpenDocumentDirectory() / sceneFilepath);
	newScene->Load();
	return newScene;
}

void BindScene(sol::state& state)
{
	PROFILE_FUNCTION();

	SetFunction(state, "ChangeScene", "Load and change to scene", &ChangeScene);
	SetFunction(state, "LoadScene", "Load a scene", &LoadScene);

	sol::usertype<Scene> scene_type = state.new_usertype<Scene>("Scene");
	scene_type.set_function("CreateEntity", static_cast<Entity(Scene::*)(const std::string&)>(&Scene::CreateEntity));
	scene_type.set_function("RemoveEntity", &Scene::RemoveEntity);
	scene_type.set_function("GetPrimaryCamera", &Scene::GetPrimaryCameraEntity);
	scene_type.set_function("FindEntity", &Scene::GetEntityByPath);
	scene_type.set_function("InstantiateScene", &Scene::InstantiateScene);
	scene_type.set_function("InstantiateEntity", &Scene::InstantiateEntity);

	sol::usertype<HitResult2D> hitResult_type = state.new_usertype<HitResult2D>("HitResult2D");
	hitResult_type["Hit"] = &HitResult2D::hit;
	hitResult_type["Entity"] = &HitResult2D::entity;
	hitResult_type["Point"] = &HitResult2D::hitPoint;
	hitResult_type["Normal"] = &HitResult2D::hitNormal;

	scene_type.set_function("RayCast2D", &Scene::RayCast2D);
	scene_type.set_function("MultiRayCast2D", &Scene::MultiRayCast2D);

	sol::table assetManager = state.create_table("AssetManager");
	assetManager.set_function("GetTexture", [](std::string_view path) -> Ref<Texture2D>
		{
			return AssetManager::GetTexture(std::filesystem::absolute(Application::GetOpenDocumentDirectory() / path));
		});
	assetManager.set_function("GetMaterial", [](std::string_view path) -> Ref<Material>
		{
			return AssetManager::GetAsset<Material>(std::filesystem::absolute(Application::GetOpenDocumentDirectory() / path));
		});
	assetManager.set_function("GetStaticMesh", [](std::string_view path) -> Ref<StaticMesh>
		{
			return AssetManager::GetAsset<StaticMesh>(std::filesystem::absolute(Application::GetOpenDocumentDirectory() / path));
		});
	assetManager.set_function("GetPhysicsMaterial", [](std::string_view path) -> Ref<PhysicsMaterial>
		{
			return AssetManager::GetAsset<PhysicsMaterial>(std::filesystem::absolute(Application::GetOpenDocumentDirectory() / path));
		});
	assetManager.set_function("GetTileset", [](std::string_view path) -> Ref<Tileset>
		{
			return AssetManager::GetAsset<Tileset>(std::filesystem::absolute(Application::GetOpenDocumentDirectory() / path));
		});

	sol::usertype<Material> material_type = state.new_usertype<Material>("Material");
	material_type.set_function("SetShader", &Material::SetShader);
	material_type.set_function("GetShader", &Material::GetShader);
	material_type.set_function("AddTexture", &Material::AddTexture);
	material_type.set_function("GetTextureOffset", &Material::GetTextureOffset);
	material_type.set_function("SetTextureOffset", &Material::SetTextureOffset);
}
}