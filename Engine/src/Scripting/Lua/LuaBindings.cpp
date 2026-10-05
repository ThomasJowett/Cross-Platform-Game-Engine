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

#include <algorithm>

namespace Lua
{
entt::registry& GetSceneRegistry(Scene* scene)
{
	return scene->GetRegistry();
}

template <typename T, typename = void>
struct has_lua_bindings : std::false_type {};

template <typename T>
struct has_lua_bindings<T, std::void_t<decltype(&T::RegisterLuaBindings)>> : std::true_type {};

template<typename Component>
void RegisterComponent(sol::state& state)
{
	std::string name = type_name<Component>().data();

	name = SplitString(name, '\n')[0];

	using Handle = ComponentHandle<Component>;
	Handle::s_LuaName = name;
	sol::usertype<Handle> component_type = state.new_usertype<Handle>(name, sol::no_constructor,
		sol::meta_function::equal_to, [](const Handle& a, const Handle& b) { return a.entity == b.entity && a.scene == b.scene; });
	component_type.set_function("IsValid", [](const Handle& handle) { return handle.TryGet() != nullptr; });
	RegisterLuaApiEntry({ "IsValid", "Whether this still refers to a live component; false once its entity is destroyed or the component removed", LuaApiEntry::Kind::Function, name, "", true });

	auto entity_Type = state["Entity"].get_or_create<sol::usertype<Entity>>();

	// Kind::ComponentAccessor, not Function - these are implemented on Entity and documented
	// generically on Entity's own page (see LuaDocGenerator), not repeated on every
	// component's page as if e.g. a TilemapComponent could be added to itself.
	auto registerAccessor = [&](const std::string& functionName, const std::string& description, auto&& function)
	{
		entity_Type.set_function(functionName, std::forward<decltype(function)>(function));
		LuaManager::AddApiEntry({ functionName, description, LuaApiEntry::Kind::ComponentAccessor, name, "", true });
	};

	registerAccessor("Add" + name, "Add a " + name + " to this entity", [](Entity& entity)
		{
			entity.AddComponent<Component>();
			return Handle{ entity.GetHandle(), entity.GetScene() };
		});
	registerAccessor("Remove" + name, "Remove the " + name + " from this entity", &Entity::RemoveComponent<Component>);
	registerAccessor("Has" + name, "Check whether this entity has a " + name, &Entity::HasComponent<Component>);
	registerAccessor("GetOrAdd" + name, "Get the entity's " + name + ", adding one first if it doesn't already have one", [](Entity& entity)
		{
			entity.GetOrAddComponent<Component>();
			return Handle{ entity.GetHandle(), entity.GetScene() };
		});
	registerAccessor("Get" + name, "Get the entity's " + name + ", or nil if it doesn't have one", [](Entity& entity) -> sol::optional<Handle>
		{
			if (!entity.TryGetComponent<Component>())
				return sol::nullopt;
			return Handle{ entity.GetHandle(), entity.GetScene() };
		});

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

	// Read-only, and only the engine creates IDs, so scripts can't change an entity's ID
	sol::usertype<Uuid> uuid_type = state.new_usertype<Uuid>("UUID", sol::no_constructor,
		sol::meta_function::equal_to, [](const Uuid& a, const Uuid& b) { return a == b; },
		sol::meta_function::to_string, &Uuid::to_string);
	SetFunction(uuid_type, "UUID", "ToString", "Get the ID as a string, e.g. to save it or use it as a table key", &Uuid::to_string);
	SetFunction(uuid_type, "UUID", "FromString", "UUID.FromString(s): read back an ID from ToString, or nil if s isn't one", [](const std::string& string) -> sol::optional<Uuid>
		{
			size_t dash = string.find('-');
			if (dash == std::string::npos || dash == 0 || dash == string.size() - 1)
				return sol::nullopt;
			auto isDigits = [](const std::string& part) { return std::all_of(part.begin(), part.end(), [](unsigned char c) { return std::isdigit(c); }); };
			std::string lo = string.substr(0, dash), hi = string.substr(dash + 1);
			if (!isDigits(lo) || !isDigits(hi))
				return sol::nullopt;
			try
			{
				return Uuid(std::stoull(lo), std::stoull(hi));
			}
			catch (const std::out_of_range&)
			{
				return sol::nullopt;
			}
		});

	sol::usertype<Entity> entity_type = state.new_usertype<Entity>("Entity",
		sol::constructors<
		Entity(),
		Entity(const Entity&),
		sol::types<entt::entity, Scene*>
		>()
	);
	SetFunction(entity_type, "Entity", "IsSceneValid", "Is Valid", &Entity::IsSceneValid);
	SetFunction(entity_type, "Entity", "GetID", "Get the entity's UUID, which stays the same across saves and loads", &Entity::GetID);
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
	RegisterLuaApiEntry({ "new", "Colour.new(r, g, b, a) with values from 0 to 1, Colour.new(Colours.Red), or Colour.new() for opaque black", LuaApiEntry::Kind::Function, "Colour", "" });
	RegisterLuaApiEntry({ "r", "Red, from 0 to 1", LuaApiEntry::Kind::Property, "Colour", "number" });
	RegisterLuaApiEntry({ "g", "Green, from 0 to 1", LuaApiEntry::Kind::Property, "Colour", "number" });
	RegisterLuaApiEntry({ "b", "Blue, from 0 to 1", LuaApiEntry::Kind::Property, "Colour", "number" });
	RegisterLuaApiEntry({ "a", "Alpha, from 0 (transparent) to 1 (opaque)", LuaApiEntry::Kind::Property, "Colour", "number" });
	SetFunction(colour_type, "Colour", "SetHexCode", "Set from a hex string such as \"#FF8800\"", static_cast<void(Colour::*)(const std::string&)>(&Colour::SetColour));
	SetFunction(colour_type, "Colour", "SetHexValue", "Set from a hex number such as 0xFF8800FF", static_cast<void(Colour::*)(const uint32_t&)>(&Colour::SetColour));
	SetFunction(colour_type, "Colour", "HexCode", "Get as a hex string", &Colour::HexCode);
	SetFunction(colour_type, "Colour", "HexValue", "Get as a hex number", &Colour::HexValue);

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
	SetEnum(state, "Colours", "Named colours, for Colour.new(Colours.Red) or colour:SetColour; Random picks one at random", coloursItems);

	SetFunction(colour_type, "Colour", "SetColour", "Set to a named colour, e.g. colour:SetColour(Colours.Red)", static_cast<void(Colour::*)(Colours)>(&Colour::SetColour));

	sol::usertype<BehaviourTree::Blackboard> blackboard_type = state.new_usertype<BehaviourTree::Blackboard>("Blackboard");
	SetFunction(blackboard_type, "Blackboard", "SetBool", "Set a boolean: (key, value)", &BehaviourTree::Blackboard::setBool);
	SetFunction(blackboard_type, "Blackboard", "SetInt", "Set an integer: (key, value)", &BehaviourTree::Blackboard::setInt);
	SetFunction(blackboard_type, "Blackboard", "SetFloat", "Set a number: (key, value)", &BehaviourTree::Blackboard::setFloat);
	SetFunction(blackboard_type, "Blackboard", "SetDouble", "Set a double: (key, value)", &BehaviourTree::Blackboard::setDouble);
	SetFunction(blackboard_type, "Blackboard", "SetString", "Set a string: (key, value)", &BehaviourTree::Blackboard::setString);
	SetFunction(blackboard_type, "Blackboard", "SetVec2", "Set a Vec2: (key, value)", &BehaviourTree::Blackboard::setVector2);
	SetFunction(blackboard_type, "Blackboard", "SetVec3", "Set a Vec3: (key, value)", &BehaviourTree::Blackboard::setVector3);
	SetFunction(blackboard_type, "Blackboard", "GetBool", "Get a boolean by key", &BehaviourTree::Blackboard::getBool);
	SetFunction(blackboard_type, "Blackboard", "GetInt", "Get an integer by key", &BehaviourTree::Blackboard::getInt);
	SetFunction(blackboard_type, "Blackboard", "GetFloat", "Get a number by key", &BehaviourTree::Blackboard::getFloat);
	SetFunction(blackboard_type, "Blackboard", "GetDouble", "Get a double by key", &BehaviourTree::Blackboard::getDouble);
	SetFunction(blackboard_type, "Blackboard", "GetString", "Get a string by key", &BehaviourTree::Blackboard::getString);
	SetFunction(blackboard_type, "Blackboard", "GetVec2", "Get a Vec2 by key", &BehaviourTree::Blackboard::getVector2);
	SetFunction(blackboard_type, "Blackboard", "GetVec3", "Get a Vec3 by key", &BehaviourTree::Blackboard::getVector3);

	sol::usertype<BehaviourTree::BehaviourTree> behaviourTree_type = state.new_usertype<BehaviourTree::BehaviourTree>("BehaviourTree");
	SetFunction(behaviourTree_type, "BehaviourTree", "GetBlackboard", "Get the tree's blackboard", &BehaviourTree::BehaviourTree::getBlackboard);

	std::initializer_list<std::pair<sol::string_view, int>> nodeStatusItems = {
		{ "Success", (int)BehaviourTree::Node::Status::Success },
		{ "Failure", (int)BehaviourTree::Node::Status::Failure },
		{ "Running", (int)BehaviourTree::Node::Status::Running },
		{ "Aborted", (int)BehaviourTree::Node::Status::Aborted }
	};
	SetEnum(state, "NodeStatus", "What a behaviour tree custom task returns from its update", nodeStatusItems);
}

//--------------------------------------------------------------------------------------------------------------

void BindDebug(sol::state& state)
{
	PROFILE_FUNCTION();

	sol::table debug = state.create_table("Debug");
	LuaManager::AddIdentifier("Debug", "Draw debug lines and shapes, from OnDebugRender");

	SetFunction(debug, "Debug", "DrawLine", "DrawLine(start, end, colour): a line between two Vec3 points", [](const Vector3f& start, const Vector3f& end, const Colour& colour)
		{ Renderer2D::DrawHairLine(start, end, colour); });
	SetFunction(debug, "Debug", "DrawCircle", "DrawCircle(position, radius, segments, colour): a circle outline", [](const Vector3f& position, float radius, uint32_t segments, const Colour& colour)
		{ Renderer2D::DrawHairLineCircle(position, radius, segments, colour); });
	SetFunction(debug, "Debug", "DrawRect", "DrawRect(position, size, colour): a rectangle outline; size is a Vec2", [](const Vector3f& position, const Vector2f& size, const Colour& colour)
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