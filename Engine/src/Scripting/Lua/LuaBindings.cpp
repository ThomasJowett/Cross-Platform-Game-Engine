#include "stdafx.h"
#include "LuaBindings.h"

#include "Logging/Instrumentor.h"
#include "Scene/Entity.h"
#include "Scene/Components.h"
#include "Renderer/Renderer2D.h"
#include "Physics/HitResult2D.h"
#include "Core/Settings.h"
#include "LuaManager.h"
#include "AI/BehaviourTree.h"

namespace Lua
{
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
	SetFunction(entity_type, "IsSceneValid", "Is Valid", & Entity::IsSceneValid);
	SetFunction(entity_type, "GetName", "Get Name", & Entity::GetName);
	SetFunction(entity_type, "SetName", "Set Name", & Entity::SetName);
	SetFunction(entity_type, "AddChild", "Add Child", & Entity::AddChild);
	SetFunction(entity_type, "Destroy", "Destroy", & Entity::Destroy);
	SetFunction(entity_type, "GetParent", "Get Parent", & Entity::GetParent);
	SetFunction(entity_type, "GetSibling", "Get Sibling", & Entity::GetSibling);
	SetFunction(entity_type, "GetChild", "Get first Child", & Entity::GetChild);

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
	camera_type["Camera"] = &CameraComponent::camera;
	camera_type["Primary"] = &CameraComponent::primary;
	camera_type["FixedAspectRatio"] = &CameraComponent::fixedAspectRatio;

	auto sprite_type = state["SpriteComponent"].get_or_create<sol::usertype<SpriteComponent>>();
	sprite_type["Tint"] = &SpriteComponent::tint;
	sprite_type["Texture"] = &SpriteComponent::texture;
	sprite_type["TilingFactor"] = &SpriteComponent::tilingFactor;

	auto animated_sprite_type = state["AnimatedSpriteComponent"].get_or_create<sol::usertype<AnimatedSpriteComponent>>();
	animated_sprite_type["Tint"] = &AnimatedSpriteComponent::tint;
	animated_sprite_type["SpriteSheet"] = &AnimatedSpriteComponent::spriteSheet;
	animated_sprite_type["Animation"] = &AnimatedSpriteComponent::animation;

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
	physicsMaterial_type.set_function("GetDensity", &PhysicsMaterial::GetDensity);
	physicsMaterial_type.set_function("SetDensity", &PhysicsMaterial::SetDensity);
	physicsMaterial_type.set_function("GetFriction", &PhysicsMaterial::GetFriction);
	physicsMaterial_type.set_function("SetFriction", &PhysicsMaterial::SetFriction);
	physicsMaterial_type.set_function("GetRestitution", &PhysicsMaterial::GetRestitution);
	physicsMaterial_type.set_function("SetRestitution", &PhysicsMaterial::SetRestitution);

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

	auto weldJoint2D_type = state["WeldJoint2DComponent"].get_or_create<sol::usertype<WeldJoint2DComponent>>();
	weldJoint2D_type["CollideConnected"] = &WeldJoint2DComponent::collideConnected;
	weldJoint2D_type["Damping"] = &WeldJoint2DComponent::damping;
	weldJoint2D_type["Stiffness"] = &WeldJoint2DComponent::stiffness;

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

	auto text_type = state["TextComponent"].get_or_create<sol::usertype<TextComponent>>();
	text_type["Text"] = &TextComponent::text;
	text_type["MaxWidth"] = &TextComponent::maxWidth;
	text_type["Colour"] = &TextComponent::colour;
	text_type["Font"] = &TextComponent::font;

	auto audio_source_type = state["AudioSourceComponent"].get_or_create<sol::usertype<AudioSourceComponent>>();
	audio_source_type["Clip"] = &AudioSourceComponent::audioClip;
	audio_source_type["Volume"] = &AudioSourceComponent::volume;
	audio_source_type["Pitch"] = &AudioSourceComponent::pitch;
	audio_source_type["Loop"] = &AudioSourceComponent::loop;
	audio_source_type["MinDistance"] = &AudioSourceComponent::minDistance;
	audio_source_type["MaxDistance"] = &AudioSourceComponent::maxDistance;
	audio_source_type["Rolloff"] = &AudioSourceComponent::rolloff;
	audio_source_type["Stream"] = &AudioSourceComponent::stream;
	audio_source_type.set_function("Play", &AudioSourceComponent::Play);
	audio_source_type.set_function("Pause", &AudioSourceComponent::Pause);
	audio_source_type.set_function("Stop", &AudioSourceComponent::Stop);
}

//--------------------------------------------------------------------------------------------------------------

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
		{ "Running", (int)BehaviourTree::Node::Status::Running }
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
	SetFunction(signal, "Connect", "Connects a function to a signal",
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
	SetFunction(signal, "Disconnect", "Disconnects a function from a signal",
		[&](const std::string& signalName, Entity listener)
		{
			LuaManager::GetSignalBus().Disconnect(signalName, listener);
		});
	SetFunction(signal, "Emit", "Emits a signal",
		[&](const std::string& signalName, Entity sender, sol::table data)
		{
			LuaManager::GetSignalBus().Emit(signalName, sender, data);
		});
}
}