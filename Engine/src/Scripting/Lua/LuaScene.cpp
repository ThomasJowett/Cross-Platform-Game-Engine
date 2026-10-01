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
#include "math/Vector2f.h"
#include "sol/property.hpp"

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

	SetFunction(state, "", "ChangeScene", "Load and change to scene", &ChangeScene);
	SetFunction(state, "", "LoadScene", "Load a scene", &LoadScene);

	sol::usertype<Scene> scene_type = state.new_usertype<Scene>("Scene");
	SetFunction(scene_type, "Scene", "CreateEntity", "Create an empty entity with the given name. It has no Transform until you add one", static_cast<Entity(Scene::*)(const std::string&)>(&Scene::CreateEntity));
	SetFunction(scene_type, "Scene", "RemoveEntity", "Destroy an entity and its children; returns false if it isn't in this scene", &Scene::RemoveEntity);
	SetFunction(scene_type, "Scene", "GetPrimaryCamera", "Get the entity whose Camera is marked Primary", &Scene::GetPrimaryCameraEntity);
	SetFunction(scene_type, "Scene", "FindEntity", "Find an entity by name, or by a path of names through the hierarchy such as \"HUD/Score\"; check the result with IsSceneValid()", &Scene::GetEntityByPath);
	SetFunction(scene_type, "Scene", "FindEntityByID", "Find the entity with the given UUID; check the result with IsSceneValid(), like FindEntity", &Scene::GetEntityByID);
	SetFunction(scene_type, "Scene", "InstantiateScene", "Spawn a copy of every entity in a loaded scene (see LoadScene) at a position", &Scene::InstantiateScene);
	SetFunction(scene_type, "Scene", "InstantiateEntity", "Spawn a copy of an entity, children included, at a position, and return the copy", &Scene::InstantiateEntity);
	SetFunction(scene_type, "Scene", "GetPixelsPerUnit", "Get how many texture pixels make one world unit", &Scene::GetPixelsPerUnit);

	sol::usertype<HitResult2D> hitResult_type = state.new_usertype<HitResult2D>("HitResult2D");
	SetProperty(hitResult_type, "HitResult2D", "Hit", "Whether anything was hit", "boolean", sol::property([](HitResult2D& h) { return h.hit; }, [](HitResult2D& h, bool v) { h.hit = v; }));
	SetProperty(hitResult_type, "HitResult2D", "Entity", "The entity that was hit", "Entity", sol::property([](HitResult2D& h) { return h.entity; }, [](HitResult2D& h, Entity v) { h.entity = v; }));
	SetProperty(hitResult_type, "HitResult2D", "Point", "World position of the hit", "Vec2", sol::property([](HitResult2D& h) { return h.hitPoint; }, [](HitResult2D& h, const Vector2f& v) { h.hitPoint = v; }));
	SetProperty(hitResult_type, "HitResult2D", "Normal", "Surface direction at the hit point", "Vec2", sol::property([](HitResult2D& h) { return h.hitNormal; }, [](HitResult2D& h, const Vector2f& v) { h.hitNormal = v; }));

	SetFunction(scene_type, "Scene", "RayCast2D", "Cast a ray between two world points and return the first collider hit, as a HitResult2D", &Scene::RayCast2D);
	SetFunction(scene_type, "Scene", "MultiRayCast2D", "Cast a ray between two world points and return every collider hit, as a table of HitResult2D", &Scene::MultiRayCast2D);
	SetFunction(scene_type, "Scene", "QueryPoint", "Get every entity whose 2D collider contains a world point, e.g. to pick objects with the mouse", &Scene::QueryPoint);
	SetFunction(scene_type, "Scene", "ScreenToWorldPoint", "Convert a screen position, such as Input.GetMousePos(), to world space using the primary camera; optional Z plane, default 0", [](Scene& scene, Vector2f screenPosition, sol::optional<float> worldZ)
		{ return scene.ScreenToWorldPoint(screenPosition, worldZ.value_or(0.0f)); });
	SetFunction(scene_type, "Scene", "WorldToScreenPoint", "Convert a world position to a screen position using the primary camera", &Scene::WorldToScreenPoint);

	sol::table assetManager = state.create_table("AssetManager");
	LuaManager::AddIdentifier("AssetManager", "Load assets by project-relative path");
	SetFunction(assetManager, "AssetManager", "GetTexture", "Load a texture by project-relative path", [](std::string_view path) -> Ref<Texture2D>
		{
			return AssetManager::GetTexture(path);
		});
	SetFunction(assetManager, "AssetManager", "GetMaterial", "Load a material by project-relative path", [](std::string_view path) -> Ref<Material>
		{
			return AssetManager::GetAsset<Material>(path);
		});
	SetFunction(assetManager, "AssetManager", "GetStaticMesh", "Load a static mesh by project-relative path", [](std::string_view path) -> Ref<StaticMesh>
		{
			return AssetManager::GetAsset<StaticMesh>(path);
		});
	SetFunction(assetManager, "AssetManager", "GetPhysicsMaterial", "Load a physics material by project-relative path", [](std::string_view path) -> Ref<PhysicsMaterial>
		{
			return AssetManager::GetAsset<PhysicsMaterial>(path);
		});
	SetFunction(assetManager, "AssetManager", "GetTileset", "Load a tileset by project-relative path", [](std::string_view path) -> Ref<Tileset>
		{
			return AssetManager::GetAsset<Tileset>(path);
		});

	sol::usertype<Material> material_type = state.new_usertype<Material>("Material");
	SetFunction(material_type, "Material", "SetShader", "Set the shader by name", &Material::SetShader);
	SetFunction(material_type, "Material", "GetShader", "Get the shader name", &Material::GetShader);
	SetFunction(material_type, "Material", "AddTexture", "Set the texture in a slot: (texture, slot); nil removes it", &Material::AddTexture);
	SetFunction(material_type, "Material", "GetTextureOffset", "Get the texture offset", &Material::GetTextureOffset);
	SetFunction(material_type, "Material", "SetTextureOffset", "Set the texture offset", &Material::SetTextureOffset);
	SetFunction(material_type, "Material", "GetTilingFactor", "Get how many times textures repeat", &Material::GetTilingFactor);
	SetFunction(material_type, "Material", "SetTilingFactor", "Set how many times textures repeat", &Material::SetTilingFactor);
	SetFunction(material_type, "Material", "SetTint", "Set the colour multiplied over the textures", &Material::SetTint);
	SetFunction(material_type, "Material", "GetTint", "Get the colour multiplied over the textures", &Material::GetTint);
	SetFunction(material_type, "Material", "IsTwoSided", "Whether back faces are drawn", &Material::IsTwoSided);
	SetFunction(material_type, "Material", "SetTwoSided", "Set whether back faces are drawn", &Material::SetTwoSided);
	SetFunction(material_type, "Material", "IsTransparent", "Whether the material is drawn with transparency", &Material::IsTransparent);
	SetFunction(material_type, "Material", "SetTransparency", "Set whether the material is drawn with transparency", &Material::SetTransparency);
	SetFunction(material_type, "Material", "CastsShadows", "Whether the material casts shadows", &Material::CastsShadows);
	SetFunction(material_type, "Material", "SetCastShadows", "Set whether the material casts shadows", &Material::SetCastShadows);

	state.new_usertype<PostProcessEffect>("PostProcessEffect", sol::no_constructor);

	state.new_usertype<GaussianBlurEffect>(
		"GaussianBlurEffect",
		sol::constructors<GaussianBlurEffect(float)>(),
		sol::base_classes, sol::bases<PostProcessEffect>()
	);

	sol::table postProcess = state.create_table("PostProcess");
	LuaManager::AddIdentifier("PostProcess", "Full-screen post-processing effects");
	SetFunction(postProcess, "PostProcess", "CreateEffect", "Create an effect by name: \"GaussianBlur\" takes a strength", [](const std::string& name, sol::variadic_args args) -> Ref<PostProcessEffect>
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
	SetFunction(postProcess, "PostProcess", "AddEffect", "Apply an effect to the rendered scene", [](Ref<PostProcessEffect> effect)
		{
			Renderer::AddPostProcessEffect(effect);
		});
	SetFunction(postProcess, "PostProcess", "RemoveEffect", "Stop applying an effect", [](Ref<PostProcessEffect> effect)
		{
			Renderer::RemovePostProcessEffect(effect);
		});
	SetFunction(postProcess, "PostProcess", "ClearEffects", "Remove every effect", []()
		{
			Renderer::ClearPostProcessEffects();
		});
}
}