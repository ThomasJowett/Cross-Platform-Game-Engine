#include "stdafx.h"
#include "LuaBindings.h"

#include "Logging/Logger.h"
#include "Logging/Instrumentor.h"
#include "Core/Application.h"
#include "Core/Input.h"
#include "Core/MouseButtonCodes.h"
#include "Scene/Scene.h"
#include "Scene/Entity.h"
#include "Scene/SceneManager.h"

namespace Lua
{
	void BindLogging(sol::state& state)
	{
		PROFILE_FUNCTION();

		sol::table log = state.create_table("Log");

		log.set_function("Trace", [&](sol::this_state s, std::string_view message)
			{
				CLIENT_TRACE(message);
			});

		log.set_function("Info", [&](sol::this_state s, std::string_view message)
			{
				CLIENT_INFO(message);
			});

		log.set_function("Debug", [&](sol::this_state s, std::string_view message)
			{
				CLIENT_DEBUG(message);
			});

		log.set_function("Warn", [&](sol::this_state s, std::string_view message)
			{
				CLIENT_WARN(message);
			});

		log.set_function("Error", [&](sol::this_state s, std::string_view message)
			{
				CLIENT_ERROR(message);
			});

		log.set_function("Critical", [&](sol::this_state s, std::string_view message)
			{
				CLIENT_CRITICAL(message);
			});
	}

	//--------------------------------------------------------------------------------------------------------------

	void BindApp(sol::state& state)
	{
		PROFILE_FUNCTION();

		sol::table application = state.create_table("App");

		application.set_function("ShowImGui", &Application::ShowImGui);
		application.set_function("ToggleImGui", &Application::ToggleImGui);

		application.set_function("GetFixedUpdateInterval", [&](sol::this_state s) -> float
			{
				return Application::Get().GetFixedUpdateInterval();
			});
	}

	//--------------------------------------------------------------------------------------------------------------

	void ChangeScene(std::string sceneFilepath)
	{
		SceneManager::ChangeScene(std::filesystem::path(sceneFilepath));
	}

	void BindScene(sol::state& state)
	{
		PROFILE_FUNCTION();

		state.set_function("ChangeScene", &ChangeScene);

		sol::usertype<Scene> scene_type = state.new_usertype<Scene>("Scene");
		scene_type.set_function("CreateEntity", static_cast<Entity(Scene::*)(const std::string&)>(&Scene::CreateEntity));
		scene_type.set_function("GetName", &Scene::GetSceneName);
		scene_type.set_function("SetName", &Scene::SetSceneName);
	}

	//--------------------------------------------------------------------------------------------------------------

	void BindEntity(sol::state& state)
	{
		PROFILE_FUNCTION();

		sol::usertype<Entity> entity_Type = state.new_usertype<Entity>("Entity");
		entity_Type.set_function("IsValid", &Entity::IsValid);
		entity_Type.set_function("GetName", &Entity::GetName);
	}

	//--------------------------------------------------------------------------------------------------------------

	void BindInput(sol::state& state)
	{
		PROFILE_FUNCTION();

		sol::table input = state.create_table("Input");

		input.set_function("IsKeyPressed", [&](char c) -> bool
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
	}

	//--------------------------------------------------------------------------------------------------------------

	void BindMath(sol::state& state)
	{
		PROFILE_FUNCTION();

		sol::usertype<Vector2f> vector2_type = state.new_usertype<Vector2f>(
			"Vec2",
			sol::constructors<Vector2f(float, float)>(),
			"x", &Vector2f::x,
			"y", &Vector2f::y,
			sol::meta_function::addition, [](const Vector2f& a, const Vector2f& b) { return a + b; },
			sol::meta_function::subtraction, [](const Vector2f& a, const Vector2f& b) { return a - b; }
		);

		vector2_type.set_function("Length", &Vector2f::Magnitude);
		vector2_type.set_function("SqrLength", &Vector2f::SqrMagnitude);
		vector2_type.set_function("Normalize", &Vector2f::Normalize);
		vector2_type.set_function("Clamp", &Vector2f::Clamp);
		vector2_type.set_function("Perpendicular", &Vector2f::Perpendicular);

		sol::usertype<Vector3f> vector3_type = state.new_usertype<Vector3f>(
			"Vec3",
			sol::constructors<Vector3f(float, float, float)>(),
			"x", &Vector3f::x,
			"y", &Vector3f::y,
			"z", &Vector3f::z,
			sol::meta_function::addition, [](const Vector3f& a, const Vector3f& b) { return a + b; },
			sol::meta_function::subtraction, [](const Vector3f& a, const Vector3f& b) { return a - b; }
		);

		vector3_type.set_function("Length", &Vector3f::Magnitude);
		vector3_type.set_function("SqrLength", &Vector3f::SqrMagnitude);
		vector3_type.set_function("Normalize", &Vector3f::Normalize);
	}
}