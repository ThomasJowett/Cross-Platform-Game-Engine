#include "stdafx.h"
#include "LuaBindings.h"

#include "Logging/Logger.h"
#include "Logging/Instrumentor.h"
#include "Core/Application.h"
#include "Core/Input.h"
#include "Core/MouseButtonCodes.h"
#include "Core/Joysticks.h"
#include "Scene/Scene.h"
#include "Scene/Entity.h"
#include "Scene/SceneManager.h"
#include "Scene/Components/Components.h"
#include "Utilities/StringUtils.h"
#include "Renderer/Renderer2D.h"
#include "Physics/HitResult2D.h"
#include "Core/Settings.h"

template<typename Component>
void RegisterComponent(sol::state& state)
{
	std::string name = type_name<Component>().data();

	name = SplitString(name, '\n')[0];
	sol::usertype<Component> component_type = state.new_usertype<Component>(name);
	auto entity_Type = state["Entity"].get_or_create<sol::usertype<Entity>>();
	entity_Type.set_function("Add" + name, static_cast<Component & (Entity::*)()>(&Entity::AddComponent<Component>));
	entity_Type.set_function("Remove" + name, &Entity::RemoveComponent<Component>);
	entity_Type.set_function("Has" + name, &Entity::HasComponent<Component>);
	entity_Type.set_function("GetOrAdd" + name, &Entity::GetOrAddComponent<Component>);
	entity_Type.set_function("Get" + name, &Entity::TryGetComponent<Component>);
}

template<typename... Component>
void RegisterAllComponents(sol::state& state)
{
	(RegisterComponent<Component>(state), ...);
}

namespace Lua
{
void BindLogging(sol::state& state)
{
	PROFILE_FUNCTION();

	sol::table log = state.create_table("Log");

	log.set_function("Trace", [](std::string_view message) { CLIENT_TRACE(message); });
	log.set_function("Info", [](std::string_view message) { CLIENT_INFO(message); });
	log.set_function("Debug", [](std::string_view message) { CLIENT_DEBUG(message); });
	log.set_function("Warn", [](std::string_view message) { CLIENT_WARN(message); });
	log.set_function("Error", [](std::string_view message) { CLIENT_ERROR(message); });
	log.set_function("Critical", [](std::string_view message) { CLIENT_CRITICAL(message); });
}

//--------------------------------------------------------------------------------------------------------------

void BindApp(sol::state& state)
{
	PROFILE_FUNCTION();

	std::initializer_list<std::pair<sol::string_view, int>> windowModes =
	{
		{ "Windowed", (int)WindowMode::WINDOWED },
		{ "Full_Screen", (int)WindowMode::FULL_SCREEN },
		{ "Borderless", (int)WindowMode::BORDERLESS }
	};
	state.new_enum("WindowMode", windowModes);

	sol::table application = state.create_table("App");

	application.set_function("ShowImGui", &Application::ShowImGui);
	application.set_function("ToggleImGui", &Application::ToggleImGui);

	application.set_function("GetFixedUpdateInterval", [](sol::this_state s)
		{ return Application::Get().GetFixedUpdateInterval(); });

	application.set_function("MaximizeWindow", [](sol::this_state s)
		{ return Application::GetWindow().MaximizeWindow(); });
	application.set_function("RestoreWindow", [](sol::this_state s)
		{ return Application::GetWindow().RestoreWindow(); });
	application.set_function("SetWindowMode", [](sol::this_state s, WindowMode windowMode)
		{ return Application::GetWindow().SetWindowMode(windowMode); });

	application.set_function("GetDocumentDirectory", []()
		{ return Application::GetOpenDocumentDirectory().string(); });

	sol::table settings = state.create_table("Settings");

	settings.set_function("SetValue", &Settings::SetValue);
	settings.set_function("SetBool", &Settings::SetBool);
	settings.set_function("SetDouble", &Settings::SetDouble);
	settings.set_function("SetInt", &Settings::SetInt);
	settings.set_function("SetVec2", &Settings::SetVector2f);
	settings.set_function("SetVec3", &Settings::SetVector3f);

	settings.set_function("GetValue", &Settings::GetValue);
	settings.set_function("GetBool", &Settings::GetBool);
	settings.set_function("GetDouble", &Settings::GetDouble);
	settings.set_function("GetInt", &Settings::GetInt);
	settings.set_function("GetVec2", &Settings::GetVector2f);
	settings.set_function("GetVec3", &Settings::GetVector3f);

	settings.set_function("SetDefaultValue", &Settings::SetDefaultValue);
	settings.set_function("SetDefaultBool", &Settings::SetDefaultBool);
	settings.set_function("SetDefaultDouble", &Settings::SetDefaultDouble);
	settings.set_function("SetDefaultInt", &Settings::SetDefaultInt);
	settings.set_function("SetDefaultVec2", &Settings::SetDefaultVector2f);
	settings.set_function("SetDefaultVec3", &Settings::SetDefaultVector3f);

	settings.set_function("GetDefaultValue", &Settings::GetDefaultValue);
	settings.set_function("GetDefaultBool", &Settings::GetDefaultBool);
	settings.set_function("GetDefaultDouble", &Settings::GetDefaultDouble);
	settings.set_function("GetDefaultInt", &Settings::GetDefaultInt);
	settings.set_function("GetDefaultVec2", &Settings::GetDefaultVector2f);
	settings.set_function("GetDefaultVec3", &Settings::GetDefaultVector3f);
}

//--------------------------------------------------------------------------------------------------------------

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

	state.set_function("ChangeScene", &ChangeScene);
	state.set_function("LoadScene", &LoadScene);

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

//--------------------------------------------------------------------------------------------------------------

void BindEntity(sol::state& state)
{
	PROFILE_FUNCTION();

	sol::usertype<Entity> entity_type = state.new_usertype<Entity>("Entity",
		sol::constructors<sol::types<entt::entity, Scene*>>());
	entity_type.set_function("IsValid", &Entity::IsValid);
	entity_type.set_function("GetName", &Entity::GetName);
	entity_type.set_function("SetName", &Entity::SetName);
	entity_type.set_function("AddChild", &Entity::AddChild);
	entity_type.set_function("Destroy", &Entity::Destroy);
	entity_type.set_function("GetParent", &Entity::GetParent);
	entity_type.set_function("GetSibling", &Entity::GetSibling);
	entity_type.set_function("GetChild", &Entity::GetChild);

	RegisterAllComponents<COMPONENTS>(state);

	auto transform_type = state["TransformComponent"].get_or_create<sol::usertype<TransformComponent>>();
	transform_type["Position"] = &TransformComponent::position;
	transform_type["Rotation"] = &TransformComponent::rotation;
	transform_type["Scale"] = &TransformComponent::scale;
	transform_type.set_function("GetWorldPosition", &TransformComponent::GetWorldPosition);

	sol::usertype<SceneCamera> sceneCamera_type = state.new_usertype<SceneCamera>("Camera");
	sceneCamera_type.set_function("SetOrthographic", &SceneCamera::SetOrthographic);
	sceneCamera_type.set_function("SetPerspective", &SceneCamera::SetPerspective);
	sceneCamera_type.set_function("SetAspectRatio", &SceneCamera::SetAspectRatio);
	sceneCamera_type.set_function("GetAspectRatio", &SceneCamera::GetAspectRatio);
	sceneCamera_type.set_function("GetOrthoNear", &SceneCamera::GetOrthoNear);
	sceneCamera_type.set_function("SetOrthoNear", &SceneCamera::GetOrthoNear);
	sceneCamera_type.set_function("GetOrthoFar", &SceneCamera::GetOrthoFar);
	sceneCamera_type.set_function("SetOrthoFar", &SceneCamera::SetOrthoFar);
	sceneCamera_type.set_function("GetOrthoSize", &SceneCamera::GetOrthoSize);
	sceneCamera_type.set_function("SetOrthoSize", &SceneCamera::SetOrthoSize);
	sceneCamera_type.set_function("GetPerspectiveNear", &SceneCamera::GetPerspectiveNear);
	sceneCamera_type.set_function("SetPerspectiveNear", &SceneCamera::SetPerspectiveNear);
	sceneCamera_type.set_function("GetPerspectiveFar", &SceneCamera::GetPerspectiveFar);
	sceneCamera_type.set_function("SetFov", &SceneCamera::SetVerticalFov);
	sceneCamera_type.set_function("GetFov", &SceneCamera::GetVerticalFov);

	auto camera_type = state["CameraComponent"].get_or_create<sol::usertype<CameraComponent>>();
	camera_type["Camera"] = &CameraComponent::Camera;
	camera_type["Primary"] = &CameraComponent::Primary;
	camera_type["FixedAspectRatio"] = &CameraComponent::FixedAspectRatio;

	auto sprite_type = state["SpriteComponent"].get_or_create<sol::usertype<SpriteComponent>>();
	sprite_type["Tint"] = &SpriteComponent::tint;
	sprite_type["Texture"] = &SpriteComponent::texture;
	sprite_type["TilingFactor"] = &SpriteComponent::tilingFactor;

	auto animated_sprite_type = state["AnimatedSpriteComponent"].get_or_create<sol::usertype<AnimatedSpriteComponent>>();
	animated_sprite_type["Tint"] = &AnimatedSpriteComponent::tint;
	animated_sprite_type["Tileset"] = &AnimatedSpriteComponent::tileset;
	animated_sprite_type.set_function("SelectAnimation", &AnimatedSpriteComponent::SelectAnimation);

	std::initializer_list<std::pair<sol::string_view, int>> rigidBodyTypesItems =
	{
		{ "Static", (int)RigidBody2DComponent::BodyType::STATIC },
		{ "Kinematic", (int)RigidBody2DComponent::BodyType::KINEMATIC },
		{ "Dynamic", (int)RigidBody2DComponent::BodyType::DYNAMIC }
	};
	state.new_enum("BodyType", rigidBodyTypesItems);

	auto rigidBody2D_type = state["RigidBody2DComponent"].get_or_create<sol::usertype<RigidBody2DComponent>>();
	rigidBody2D_type["Type"] = &RigidBody2DComponent::type;
	rigidBody2D_type["FixedRotation"] = &RigidBody2DComponent::fixedRotation;
	rigidBody2D_type["GravityScale"] = &RigidBody2DComponent::gravityScale;
	rigidBody2D_type["AngularDamping"] = &RigidBody2DComponent::angularDamping;
	rigidBody2D_type["LinearDamping"] = &RigidBody2DComponent::linearDamping;
	rigidBody2D_type.set_function("ApplyImpulse", &RigidBody2DComponent::ApplyImpulse);
	rigidBody2D_type.set_function("ApplyImpulseAtPoint", &RigidBody2DComponent::ApplyImpulseAtPoint);
	rigidBody2D_type.set_function("ApplyForce", &RigidBody2DComponent::ApplyForce);
	rigidBody2D_type.set_function("ApplyForceAtPoint", &RigidBody2DComponent::ApplyForceAtPoint);
	rigidBody2D_type.set_function("ApplyTorque", &RigidBody2DComponent::ApplyTorque);
	rigidBody2D_type.set_function("GetLinearVelocity", &RigidBody2DComponent::GetLinearVelocity);
	rigidBody2D_type.set_function("SetLinearVelocity", &RigidBody2DComponent::SetLinearVelocity);
	rigidBody2D_type.set_function("GetAngularVelocity", &RigidBody2DComponent::GetAngularVelocity);
	rigidBody2D_type.set_function("SetAngularVelocity", &RigidBody2DComponent::SetAngularVelocity);

	auto physicsMaterial_type = state.new_usertype<PhysicsMaterial>("PhysicsMaterial");
	physicsMaterial_type["Density"] = &PhysicsMaterial::GetDensity;

	auto boxCollider2D_type = state["BoxCollider2DComponent"].get_or_create<sol::usertype<BoxCollider2DComponent>>();
	boxCollider2D_type["Offset"] = &BoxCollider2DComponent::offset;
	boxCollider2D_type["Size"] = &BoxCollider2DComponent::size;
	boxCollider2D_type["PhysicsMaterial"] = &BoxCollider2DComponent::physicsMaterial;

	auto circleCollider2D_type = state["CircleCollider2DComponent"].get_or_create<sol::usertype<CircleCollider2DComponent>>();
	circleCollider2D_type["Offset"] = &CircleCollider2DComponent::offset;
	circleCollider2D_type["Radius"] = &CircleCollider2DComponent::radius;
	circleCollider2D_type["PhysicsMaterial"] = &CircleCollider2DComponent::physicsMaterial;

	auto polygonCollider2D_type = state["PolygonCollider2DComponent"].get_or_create<sol::usertype<PolygonCollider2DComponent>>();
	polygonCollider2D_type["Offset"] = &PolygonCollider2DComponent::offset;
	polygonCollider2D_type["Vertices"] = &PolygonCollider2DComponent::vertices;
	polygonCollider2D_type["PhysicsMaterial"] = &PolygonCollider2DComponent::physicsMaterial;

	auto capsuleCollider2D_type = state["CapsuleCollider2DComponent"].get_or_create<sol::usertype<CapsuleCollider2DComponent>>();
	capsuleCollider2D_type["Offset"] = &CapsuleCollider2DComponent::offset;
	capsuleCollider2D_type["Radius"] = &CapsuleCollider2DComponent::radius;
	capsuleCollider2D_type["Height"] = &CapsuleCollider2DComponent::height;
	capsuleCollider2D_type["PhysicsMaterial"] = &CapsuleCollider2DComponent::physicsMaterial;

	auto circleRenderer_type = state["CircleRendererComponent"].get_or_create<sol::usertype<CircleRendererComponent>>();
	circleRenderer_type["Colour"] = &CircleRendererComponent::colour;
	circleRenderer_type["Radius"] = &CircleRendererComponent::radius;
	circleRenderer_type["Thickness"] = &CircleRendererComponent::thickness;
	circleRenderer_type["Fade"] = &CircleRendererComponent::fade;

	auto primitive_type = state["PrimitiveComponent"].get_or_create<sol::usertype<PrimitiveComponent>>();
	primitive_type["Type"] = &PrimitiveComponent::type;
	primitive_type["CubeWidth"] = &PrimitiveComponent::cubeWidth;
	primitive_type["CubeHeight"] = &PrimitiveComponent::cubeHeight;
	primitive_type["CubeDepth"] = &PrimitiveComponent::cubeDepth;
	primitive_type["ShpereRadius"] = &PrimitiveComponent::sphereRadius;
	primitive_type["SphereLongitudeLines"] = &PrimitiveComponent::sphereLongitudeLines;
	primitive_type["SphereLatitudeLines"] = &PrimitiveComponent::sphereLatitudeLines;
	primitive_type["PlaneWidth"] = &PrimitiveComponent::planeWidth;
	primitive_type["PlaneLength"] = &PrimitiveComponent::planeLength;
	primitive_type["PlaneWidthLines"] = &PrimitiveComponent::planeWidthLines;
	primitive_type["PlaneLengthLines"] = &PrimitiveComponent::planeLengthLines;
	primitive_type["PlaneTileU"] = &PrimitiveComponent::planeTileU;
	primitive_type["PlaneTileV"] = &PrimitiveComponent::planeTileV;
	primitive_type["ConeBottomRadius"] = &PrimitiveComponent::coneBottomRadius;
	primitive_type["ConeHeight"] = &PrimitiveComponent::coneHeight;
	primitive_type["ConeSliceCount"] = &PrimitiveComponent::coneSliceCount;
	primitive_type["ConeStackCount"] = &PrimitiveComponent::coneStackCount;
	primitive_type["CylinderBottomRadius"] = &PrimitiveComponent::cylinderBottomRadius;
	primitive_type["CylinderTopRadius"] = &PrimitiveComponent::cylinderTopRadius;
	primitive_type["CylinderHeight"] = &PrimitiveComponent::cylinderHeight;
	primitive_type["CylinderSliceCount"] = &PrimitiveComponent::cylinderSliceCount;
	primitive_type["CylinderStackCount"] = &PrimitiveComponent::cylinderStackCount;
	primitive_type["TorusOuterRadius"] = &PrimitiveComponent::torusOuterRadius;
	primitive_type["TorusInnerRadius"] = &PrimitiveComponent::torusInnerRadius;
	primitive_type["TorusSliceCount"] = &PrimitiveComponent::torusSliceCount;
	primitive_type["Material"] = &PrimitiveComponent::material;
	primitive_type["Mesh"] = &PrimitiveComponent::mesh;
	primitive_type.set_function("SetCube", &PrimitiveComponent::SetCube);
	primitive_type.set_function("SetSphere", &PrimitiveComponent::SetSphere);
	primitive_type.set_function("SetPlane", &PrimitiveComponent::SetPlane);
	primitive_type.set_function("SetCylinder", &PrimitiveComponent::SetCylinder);
	primitive_type.set_function("SetCone", &PrimitiveComponent::SetCone);
	primitive_type.set_function("SetTorus", &PrimitiveComponent::SetTorus);
	primitive_type.set_function("SetType", &PrimitiveComponent::SetType);
}

//--------------------------------------------------------------------------------------------------------------

void BindInput(sol::state& state)
{
	PROFILE_FUNCTION();

	sol::table input = state.create_table("Input");

	input.set_function("IsKeyPressed", [](char c)
		{
			return Input::IsKeyPressed((int)c);
		});
	input.set_function("IsMouseButtonPressed", &Input::IsMouseButtonPressed);
	input.set_function("GetMousePos", &Input::GetMousePos);

	std::initializer_list<std::pair<sol::string_view, int>> mouseItems = {
		{ "Left", MOUSE_BUTTON_LEFT },
		{ "Right", MOUSE_BUTTON_RIGHT },
		{ "Middle", MOUSE_BUTTON_MIDDLE },
	};
	state.new_enum("MouseButton", mouseItems);

	std::initializer_list<std::pair<sol::string_view, int>> joystickItems = {
		{ "A", GAMEPAD_BUTTON_A },
		{ "B", GAMEPAD_BUTTON_B },
		{ "X", GAMEPAD_BUTTON_X },
		{ "Y", GAMEPAD_BUTTON_Y },
		{ "LeftBumper", GAMEPAD_BUTTON_LEFT_BUMPER },
		{ "RightBumper", GAMEPAD_BUTTON_LEFT_BUMPER },
		{ "Back", GAMEPAD_BUTTON_BACK },
		{ "Start", GAMEPAD_BUTTON_START },
		{ "Guide",GAMEPAD_BUTTON_GUIDE },
		{ "LeftThumbStick", GAMEPAD_BUTTON_LEFT_THUMB },
		{ "RightThumbStick", GAMEPAD_BUTTON_RIGHT_THUMB },
		{ "Up", GAMEPAD_BUTTON_DPAD_UP },
		{ "Right", GAMEPAD_BUTTON_DPAD_RIGHT },
		{ "Down", GAMEPAD_BUTTON_DPAD_DOWN },
		{ "Left", GAMEPAD_BUTTON_DPAD_LEFT },
		{ "Cross", GAMEPAD_BUTTON_CROSS },
		{ "Circle", GAMEPAD_BUTTON_CIRCLE },
		{ "Square", GAMEPAD_BUTTON_SQUARE },
		{ "Triangle", GAMEPAD_BUTTON_TRIANGLE }
	};
	state.new_enum("JoystickButton", joystickItems);

	std::initializer_list<std::pair<sol::string_view, int>> joystickAxisItems =
	{
		{ "LeftX", GAMEPAD_AXIS_LEFT_X },
		{ "LeftY", GAMEPAD_AXIS_LEFT_Y },
		{ "RightX", GAMEPAD_AXIS_RIGHT_X },
		{ "RightY", GAMEPAD_AXIS_RIGHT_Y },
		{ "LeftTrigger", GAMEPAD_AXIS_LEFT_TRIGGER },
		{ "RightTrigger", GAMEPAD_AXIS_RIGHT_TRIGGER }
	};
	state.new_enum("JoystickAxis", joystickAxisItems);

	input.set_function("GetJoyStickCount", &Joysticks::GetJoystickCount);
	input.set_function("IsJoystickButtonPressed", &Input::IsJoystickButtonPressed);
	input.set_function("GetJoystickAxis", &Input::GetJoystickAxis);
}

//--------------------------------------------------------------------------------------------------------------

void BindMath(sol::state& state)
{
	PROFILE_FUNCTION();

	sol::usertype<Vector2f> vector2_type = state.new_usertype<Vector2f>(
		"Vec2",
		sol::constructors<Vector2f(float, float), Vector2f()>(),
		"x", &Vector2f::x,
		"y", &Vector2f::y,
		sol::meta_function::addition, [](const Vector2f& a, const Vector2f& b) { return a + b; },
		sol::meta_function::subtraction, [](const Vector2f& a, const Vector2f& b) { return a - b; },
		sol::meta_function::multiplication, [](const Vector2f& a, const float& b) {return a * b; },
		sol::meta_function::unary_minus, [](Vector2f const& a) {return -a; }
	);

	vector2_type.set_function("Length", &Vector2f::Magnitude);
	vector2_type.set_function("SqrLength", &Vector2f::SqrMagnitude);
	vector2_type.set_function("Normalize", &Vector2f::Normalize);
	vector2_type.set_function("Clamp", &Vector2f::Clamp);
	vector2_type.set_function("Perpendicular", &Vector2f::Perpendicular);

	sol::usertype<Vector3f> vector3_type = state.new_usertype<Vector3f>(
		"Vec3",
		sol::constructors<Vector3f(float, float, float), Vector3f()>(),
		"x", &Vector3f::x,
		"y", &Vector3f::y,
		"z", &Vector3f::z,
		sol::meta_function::addition, [](const Vector3f& a, const Vector3f& b) { return a + b; },
		sol::meta_function::subtraction, [](const Vector3f& a, const Vector3f& b) { return a - b; },
		sol::meta_function::multiplication, [](const Vector3f& a, const float& b) {return a * b; },
		sol::meta_function::unary_minus, [](Vector3f const& a) {return -a; }
	);

	vector3_type.set_function("Length", &Vector3f::Magnitude);
	vector3_type.set_function("SqrLength", &Vector3f::SqrMagnitude);
	vector3_type.set_function("Normalize", &Vector3f::Normalize);

	sol::usertype<Quaternion> quaternion_type = state.new_usertype<Quaternion>(
		"Quaternion",
		sol::constructors<Quaternion(float, float, float), Quaternion()>(),
		"w", &Quaternion::w,
		"x", &Quaternion::x,
		"y", &Quaternion::y,
		"z", &Quaternion::z,
		sol::meta_function::addition, [](const Quaternion& a, const Quaternion& b) { return a + b; },
		sol::meta_function::addition, [](const Quaternion& a, const Quaternion& b) { return a - b; }
	);

	quaternion_type.set_function("EulerAngles", &Quaternion::EulerAngles);
	quaternion_type.set_function("Length", &Quaternion::GetMagnitude);
	quaternion_type.set_function("SqrLength", &Quaternion::GetSqrMagnitude);
	quaternion_type.set_function("Normalize", &Quaternion::Normalize);
	quaternion_type.set_function("GetNormalized", &Quaternion::GetNormalized);
	quaternion_type.set_function("Conjugate", &Quaternion::Conjugate);
	quaternion_type.set_function("Inverse", &Quaternion::Inverse);
}

void BindCommonTypes(sol::state& state)
{
	sol::usertype<Colour> colour_type = state.new_usertype<Colour>(
		"Colour",
		sol::constructors<Colour(float, float, float, float), Colour(), Colour(Colours)>(),
		"r", &Colour::r,
		"g", &Colour::g,
		"b", &Colour::b,
		"a", &Colour::a
		);
	colour_type.set_function("SetHexCode", static_cast<void(Colour::*)(const std::string&)>(&Colour::SetColour));
	colour_type.set_function("SetHexValue", static_cast<void(Colour::*)(const uint32_t&)>(&Colour::SetColour));
	colour_type.set_function("HexCode", &Colour::HexCode);
	colour_type.set_function("HexValue", &Colour::HexValue);

	std::initializer_list<std::pair<sol::string_view, int>> coloursItems = {
		{"Beige", (int)Colours::BEIGE},
		{"Black", (int)Colours::BLACK},
		{"Blue", (int)Colours::BLUE},
		{"Brown", (int)Colours::BROWN},
		{"Cyan", (int)Colours::CYAN},
		{"ForestGreen", (int)Colours::FOREST_GREEN},
		{"Green", (int)Colours::GREEN},
		{"Grey", (int)Colours::GREY},
		{"Indigo", (int)Colours::INDIGO},
		{"Khaki", (int)Colours::KHAKI},
		{"LimeGreen", (int)Colours::LIME_GREEN},
		{"Magenta", (int)Colours::MAGENTA},
		{"Maroon", (int)Colours::MAROON},
		{"Mustard", (int)Colours::MUSTARD},
		{"Navy", (int)Colours::NAVY},
		{"Olive", (int)Colours::OLIVE},
		{"Orange", (int)Colours::ORANGE},
		{"Pink", (int)Colours::PINK},
		{"Purple", (int)Colours::PURPLE},
		{"Red", (int)Colours::RED},
		{"Silver", (int)Colours::SILVER},
		{"Teal", (int)Colours::TEAL},
		{"Turquoise", (int)Colours::TURQUOISE},
		{"Violet", (int)Colours::VIOLET},
		{"White", (int)Colours::WHITE},
		{"Yellow", (int)Colours::YELLOW},
		{"Random", (int)Colours::RANDOM}
	};
	state.new_enum("Colours", coloursItems);

	colour_type.set_function("SetColour", static_cast<void(Colour::*)(Colours)>(&Colour::SetColour));
}

void BindDebug(sol::state& state)
{
	PROFILE_FUNCTION();

	sol::table debug = state.create_table("Debug");

	debug.set_function("DrawLine", [](const Vector3f& start, const Vector3f& end, const Colour& colour)
		{ Renderer2D::DrawHairLine(start, end, colour); });
	debug.set_function("DrawCircle", [](const Vector3f& position, float radius, uint32_t segments, const Colour& colour)
		{ Renderer2D::DrawHairLineCircle(position, radius, segments, colour); });
	debug.set_function("DrawRect", [](const Vector3f& position, const Vector2f& size, const Colour& colour)
		{ Renderer2D::DrawHairLineRect(position, size, colour); });
}
}