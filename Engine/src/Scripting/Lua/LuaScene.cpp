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
#include "Renderer/Renderer.h"

#include "Renderer/PostProcessEffects/GaussianBlurEffect.h"

namespace Lua
{
void ChangeScene(const std::string_view sceneFilepath)
{
	SceneManager::ChangeScene(std::filesystem::path(sceneFilepath));
}

Ref<Scene> LoadScene(const std::string_view sceneFilepath)
{
	Ref<Scene> newScene = CreateRef<Scene>(Application::GetOpenDocumentDirectory() / sceneFilepath);
	std::filesystem::path scenePath = Application::GetOpenDocumentDirectory() / sceneFilepath;
	if (std::filesystem::exists(scenePath))
	{
		newScene->Load();
		return newScene;
	}
	else if (AssetManager::HasBundle())
	{
		std::vector<uint8_t> data;
		if (AssetManager::GetFileData(sceneFilepath, data))
		{
			newScene->Load(data);
			return newScene;
		}
		else
		{
			ENGINE_ERROR("Failed to load scene from bundle: {0}", sceneFilepath);
			return nullptr;
		}
	}
	ENGINE_ERROR("Scene file does not exist: {0}", sceneFilepath);
	return nullptr;
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
	scene_type.set_function("GetPixelsPerUnit", &Scene::GetPixelsPerUnit);

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
			return AssetManager::GetTexture(path);
		});
	assetManager.set_function("GetMaterial", [](std::string_view path) -> Ref<Material>
		{
			return AssetManager::GetAsset<Material>(path);
		});
	assetManager.set_function("GetStaticMesh", [](std::string_view path) -> Ref<StaticMesh>
		{
			return AssetManager::GetAsset<StaticMesh>(path);
		});
	assetManager.set_function("GetPhysicsMaterial", [](std::string_view path) -> Ref<PhysicsMaterial>
		{
			return AssetManager::GetAsset<PhysicsMaterial>(path);
		});
	assetManager.set_function("GetTileset", [](std::string_view path) -> Ref<Tileset>
		{
			return AssetManager::GetAsset<Tileset>(path);
		});

	sol::usertype<Material> material_type = state.new_usertype<Material>("Material");
	material_type.set_function("SetShader", &Material::SetShader);
	material_type.set_function("GetShader", &Material::GetShader);
	material_type.set_function("AddTexture", &Material::AddTexture);
	material_type.set_function("GetTextureOffset", &Material::GetTextureOffset);
	material_type.set_function("SetTextureOffset", &Material::SetTextureOffset);
	material_type.set_function("GetTilingFactor", &Material::GetTilingFactor);
	material_type.set_function("SetTilingFactor", &Material::SetTilingFactor);
	material_type.set_function("SetTint", &Material::SetTint);
	material_type.set_function("GetTint", &Material::GetTint);
	material_type.set_function("IsTwoSided", &Material::IsTwoSided);
	material_type.set_function("SetTwoSided", &Material::SetTwoSided);
	material_type.set_function("IsTransparent", &Material::IsTransparent);
	material_type.set_function("SetTransparency", &Material::SetTransparency);
	material_type.set_function("CastsShadows", &Material::CastsShadows);
	material_type.set_function("SetCastShadows", &Material::SetCastShadows);

	state.new_usertype<PostProcessEffect>("PostProcessEffect", sol::no_constructor);

	state.new_usertype<GaussianBlurEffect>(
		"GaussianBlurEffect",
		sol::constructors<GaussianBlurEffect(float)>(),
		sol::base_classes, sol::bases<PostProcessEffect>()
	);
	
	sol::table postProcess = state.create_table("PostProcess");
	postProcess.set_function("CreateEffect", [](const std::string& name, sol::variadic_args args) -> Ref<PostProcessEffect> 
		{
			if (name == "GaussianBlur") {
				if (args.size() == 1) {
					float strength = args.get<float>(0);
					return std::make_shared<GaussianBlurEffect>(strength);
				}
				else {
					ENGINE_ERROR("GaussianBlur effect requires 1 argument (strength)");
					return nullptr;
				}
			}
			else {
				ENGINE_ERROR("Unkown effect type: {}", name);
				return nullptr;
			}
		});
	postProcess.set_function("AddEffect", [](Ref<PostProcessEffect> effect)
		{
			Renderer::AddPostProcessEffect(effect);
		});
	postProcess.set_function("RemoveEffect", [](Ref<PostProcessEffect> effect)
		{
			Renderer::RemovePostProcessEffect(effect);
		});
	postProcess.set_function("ClearEffects", []()
		{
			Renderer::ClearPostProcessEffects();
		});
}
}