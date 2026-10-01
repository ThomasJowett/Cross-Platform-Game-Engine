#include "LuaBindings.h"

#include "Asset/Texture.h"
#include "Core/Colour.h"
#include "Logging/Instrumentor.h"
#include "Scene/Components/CameraComponent.h"
#include "Scene/Components/SpriteComponent.h"
#include "Scene/Components/TransformComponent.h"
#include "Scene/Entity.h"
#include "Scene/Components.h"
#include "Renderer/Renderer2D.h"
#include "Physics/HitResult2D.h"
#include "Core/Settings.h"
#include "LuaManager.h"
#include "AI/BehaviourTree.h"
#include "Scene/SceneCamera.h"
#include "math/Vector3f.h"

namespace Lua
{
template <typename T, typename = void>
struct has_lua_bindings : std::false_type {};

template <typename T>
struct has_lua_bindings<T, std::void_t<decltype(&T::RegisterLuaBindings)>> : std::true_type {};

template<typename Component>
void RegisterComponent(sol::state& state)
{
	std::string name = type_name<Component>().data();

	name = SplitString(name, '\n')[0];
	sol::usertype<Component> component_type = state.new_usertype<Component>(name);
	auto entity_Type = state["Entity"].get_or_create<sol::usertype<Entity>>();

	// Kind::ComponentAccessor, not Function - these are implemented on Entity and documented
	// generically on Entity's own page (see LuaDocGenerator), not repeated on every
	// component's page as if e.g. a TilemapComponent could be added to itself.
	auto registerAccessor = [&](const std::string& functionName, const std::string& description, auto&& function)
	{
		entity_Type.set_function(functionName, std::forward<decltype(function)>(function));
		LuaManager::AddApiEntry({ functionName, description, LuaApiEntry::Kind::ComponentAccessor, name, "", true });
	};

	registerAccessor("Add" + name, "Add a " + name + " to this entity", static_cast<Component & (Entity::*)()>(&Entity::AddComponent<Component>));
	registerAccessor("Remove" + name, "Remove the " + name + " from this entity", &Entity::RemoveComponent<Component>);
	registerAccessor("Has" + name, "Check whether this entity has a " + name, &Entity::HasComponent<Component>);
	registerAccessor("GetOrAdd" + name, "Get the entity's " + name + ", adding one first if it doesn't already have one", &Entity::GetOrAddComponent<Component>);
	registerAccessor("Get" + name, "Get the entity's " + name + ", or nil if it doesn't have one", &Entity::TryGetComponent<Component>);

	if constexpr (has_lua_bindings<Component>::value)
	{
		Component::RegisterLuaBindings(state, component_type);
	}
}

template<typename... Component>
void RegisterAllComponents(sol::state& state)
{
	(RegisterComponent<Component>(state), ...);
}

//--------------------------------------------------------------------------------------------------------------

void BindEntity(sol::state& state)
{
	PROFILE_FUNCTION();

	sol::usertype<Entity> entity_type = state.new_usertype<Entity>("Entity",
		sol::constructors<
		Entity(),
		Entity(const Entity&),
		sol::types<entt::entity, Scene*>
		>()
	);
	SetFunction(entity_type, "Entity", "IsSceneValid", "Is Valid", &Entity::IsSceneValid);
	SetFunction(entity_type, "Entity", "GetName", "Get Name", &Entity::GetName);
	SetFunction(entity_type, "Entity", "SetName", "Set Name", &Entity::SetName);
	SetFunction(entity_type, "Entity", "AddChild", "Add Child", &Entity::AddChild);
	SetFunction(entity_type, "Entity", "Destroy", "Destroy", &Entity::Destroy);
	SetFunction(entity_type, "Entity", "GetParent", "Get Parent", &Entity::GetParent);
	SetFunction(entity_type, "Entity", "GetSibling", "Get Sibling", &Entity::GetSibling);
	SetFunction(entity_type, "Entity", "GetChild", "Get first Child", &Entity::GetChild);

	RegisterAllComponents<COMPONENTS>(state);

	// Component bindings live in each component's REFLECT_LUA block; these are the non-component types they use
	sol::usertype<SceneCamera> sceneCamera_type = state.new_usertype<SceneCamera>("Camera");
	SetFunction(sceneCamera_type, "Camera", "SetOrthographic", "Switch to an orthographic projection: (size, near, far)", &SceneCamera::SetOrthographic);
	SetFunction(sceneCamera_type, "Camera", "SetPerspective", "Switch to a perspective projection: (verticalFov, near, far)", &SceneCamera::SetPerspective);
	SetFunction(sceneCamera_type, "Camera", "SetAspectRatio", "Set the width / height aspect ratio", &SceneCamera::SetAspectRatio);
	SetFunction(sceneCamera_type, "Camera", "GetAspectRatio", "Get the width / height aspect ratio", &SceneCamera::GetAspectRatio);
	SetFunction(sceneCamera_type, "Camera", "GetOrthoNear", "Get the orthographic near clip distance", &SceneCamera::GetOrthoNear);
	SetFunction(sceneCamera_type, "Camera", "SetOrthoNear", "Set the orthographic near clip distance", &SceneCamera::SetOrthoNear);
	SetFunction(sceneCamera_type, "Camera", "GetOrthoFar", "Get the orthographic far clip distance", &SceneCamera::GetOrthoFar);
	SetFunction(sceneCamera_type, "Camera", "SetOrthoFar", "Set the orthographic far clip distance", &SceneCamera::SetOrthoFar);
	SetFunction(sceneCamera_type, "Camera", "GetOrthoSize", "Get the orthographic view height, in world units", &SceneCamera::GetOrthoSize);
	SetFunction(sceneCamera_type, "Camera", "SetOrthoSize", "Set the orthographic view height, in world units", &SceneCamera::SetOrthoSize);
	SetFunction(sceneCamera_type, "Camera", "GetPerspectiveNear", "Get the perspective near clip distance", &SceneCamera::GetPerspectiveNear);
	SetFunction(sceneCamera_type, "Camera", "SetPerspectiveNear", "Set the perspective near clip distance", &SceneCamera::SetPerspectiveNear);
	SetFunction(sceneCamera_type, "Camera", "GetPerspectiveFar", "Get the perspective far clip distance", &SceneCamera::GetPerspectiveFar);
	SetFunction(sceneCamera_type, "Camera", "SetFov", "Set the vertical field of view, in radians", &SceneCamera::SetVerticalFov);
	SetFunction(sceneCamera_type, "Camera", "GetFov", "Get the vertical field of view, in radians", &SceneCamera::GetVerticalFov);

	auto physicsMaterial_type = state.new_usertype<PhysicsMaterial>("PhysicsMaterial");
	SetFunction(physicsMaterial_type, "PhysicsMaterial", "GetDensity", "Get the density, which sets the body's mass from its area", &PhysicsMaterial::GetDensity);
	SetFunction(physicsMaterial_type, "PhysicsMaterial", "SetDensity", "Set the density, which sets the body's mass from its area", &PhysicsMaterial::SetDensity);
	SetFunction(physicsMaterial_type, "PhysicsMaterial", "GetFriction", "Get the friction coefficient", &PhysicsMaterial::GetFriction);
	SetFunction(physicsMaterial_type, "PhysicsMaterial", "SetFriction", "Set the friction coefficient", &PhysicsMaterial::SetFriction);
	SetFunction(physicsMaterial_type, "PhysicsMaterial", "GetRestitution", "Get the bounciness (0 none, 1 fully elastic)", &PhysicsMaterial::GetRestitution);
	SetFunction(physicsMaterial_type, "PhysicsMaterial", "SetRestitution", "Set the bounciness (0 none, 1 fully elastic)", &PhysicsMaterial::SetRestitution);
}

//--------------------------------------------------------------------------------------------------------------

void BindCommonTypes(sol::state& state)
{
	sol::usertype<Colour> colour_type = state.new_usertype<Colour>(
		"Colour",
		sol::constructors<Colour(float, float, float, float), Colour(), Colour(Colours)>(),
		"r", sol::property([](Colour& c) { return c.r; }, [](Colour& c, float v) { c.r = v; }),
		"g", sol::property([](Colour& c) { return c.g; }, [](Colour& c, float v) { c.g = v; }),
		"b", sol::property([](Colour& c) { return c.b; }, [](Colour& c, float v) { c.b = v; }),
		"a", sol::property([](Colour& c) { return c.a; }, [](Colour& c, float v) { c.a = v; })
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

	sol::usertype<BehaviourTree::Blackboard> blackboard_type = state.new_usertype<BehaviourTree::Blackboard>(
		"Blackboard",
		"SetBool", &BehaviourTree::Blackboard::setBool,
		"SetInt", &BehaviourTree::Blackboard::setInt,
		"SetFloat", &BehaviourTree::Blackboard::setFloat,
		"SetDouble", &BehaviourTree::Blackboard::setDouble,
		"SetString", &BehaviourTree::Blackboard::setString,
		"SetVec2", &BehaviourTree::Blackboard::setVector2,
		"SetVec3", &BehaviourTree::Blackboard::setVector3,
		"GetBool", &BehaviourTree::Blackboard::getBool,
		"GetInt", &BehaviourTree::Blackboard::getInt,
		"GetFloat", &BehaviourTree::Blackboard::getFloat,
		"GetDouble", &BehaviourTree::Blackboard::getDouble,
		"GetString", &BehaviourTree::Blackboard::getString,
		"GetVec2", &BehaviourTree::Blackboard::getVector2,
		"GetVec3", &BehaviourTree::Blackboard::getVector3
	);

	sol::usertype<BehaviourTree::BehaviourTree> behaviourTree_type = state.new_usertype<BehaviourTree::BehaviourTree>(
		"BehaviourTree",
		"GetBlackboard", &BehaviourTree::BehaviourTree::getBlackboard
	);

	std::initializer_list<std::pair<sol::string_view, int>> nodeStatusItems = {
		{ "Success", (int)BehaviourTree::Node::Status::Success },
		{ "Failure", (int)BehaviourTree::Node::Status::Failure },
		{ "Running", (int)BehaviourTree::Node::Status::Running },
		{ "Aborted", (int)BehaviourTree::Node::Status::Aborted }
	};
	state.new_enum("NodeStatus", nodeStatusItems);
}

//--------------------------------------------------------------------------------------------------------------

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

//--------------------------------------------------------------------------------------------------------------

void BindSignaling(sol::state& state)
{
	PROFILE_FUNCTION();
	sol::table signal = state.create_table("Signal");
	LuaManager::AddIdentifier("Signal", "Signal bus");
	SetFunction(signal, "Signal", "Connect", "Connects a function to a signal",
		[&](const std::string& signalName, Entity listener, sol::function callback)
		{
			SignalBus::Callback cb = [callback](Entity sender, sol::table data)
				{
					sol::state_view lua = callback.lua_state();
					sol::table luaData = lua.create_table();

					for (const auto& [key, value] : data)
					{
						luaData[key] = value;
					}

					callback(sender, luaData);
				};
			LuaManager::GetSignalBus().Connect(signalName, listener, cb);
		});
	SetFunction(signal, "Signal", "Disconnect", "Disconnects a function from a signal",
		[&](const std::string& signalName, Entity listener)
		{
			LuaManager::GetSignalBus().Disconnect(signalName, listener);
		});
	SetFunction(signal, "Signal", "Emit", "Emits a signal",
		[&](const std::string& signalName, Entity sender, sol::table data)
		{
			LuaManager::GetSignalBus().Emit(signalName, sender, data);
		});
}
}