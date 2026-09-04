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

	auto transform_type = state["TransformComponent"].get_or_create<sol::usertype<TransformComponent>>();
	transform_type["Position"] = sol::property([](TransformComponent& c) -> Vector3f& { return c.position; }, [](TransformComponent& c, const Vector3f& v) { c.position = v; });
	transform_type["Rotation"] = sol::property([](TransformComponent& c) -> Vector3f& { return c.rotation; }, [](TransformComponent& c, const Vector3f& v) { c.rotation = v; });
	transform_type["Scale"] = sol::property([](TransformComponent& c) -> Vector3f& { return c.scale; }, [](TransformComponent& c, const Vector3f& v) { c.scale = v; });
	transform_type.set_function("GetWorldPosition", &TransformComponent::GetWorldPosition);

	sol::usertype<SceneCamera> sceneCamera_type = state.new_usertype<SceneCamera>("Camera");
	sceneCamera_type.set_function("SetOrthographic", &SceneCamera::SetOrthographic);
	sceneCamera_type.set_function("SetPerspective", &SceneCamera::SetPerspective);
	sceneCamera_type.set_function("SetAspectRatio", &SceneCamera::SetAspectRatio);
	sceneCamera_type.set_function("GetAspectRatio", &SceneCamera::GetAspectRatio);
	sceneCamera_type.set_function("GetOrthoNear", &SceneCamera::GetOrthoNear);
	sceneCamera_type.set_function("SetOrthoNear", &SceneCamera::SetOrthoNear);
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
	camera_type["Camera"] = sol::property([](CameraComponent& c) { return c.camera; }, [](CameraComponent& c, const SceneCamera& v) { c.camera = v; });
	camera_type["Primary"] = sol::property([](CameraComponent& c) { return c.primary; }, [](CameraComponent& c, const bool v) { c.primary = v; });
	camera_type["FixedAspectRatio"] = sol::property([](CameraComponent& c) { return c.fixedAspectRatio; }, [](CameraComponent& c, const bool v) { c.fixedAspectRatio = v; });

	auto sprite_type = state["SpriteComponent"].get_or_create<sol::usertype<SpriteComponent>>();
	sprite_type["Tint"] = sol::property([](SpriteComponent& c) -> Colour& { return c.tint; }, [](SpriteComponent& c, const Colour& v) { c.tint = v; });
	sprite_type["Texture"] = sol::property([](SpriteComponent& c) { return c.texture; }, [](SpriteComponent& c, const Ref<Texture2D>& v) { c.texture = v; c.texturePath = v ? v->GetFilepath() : std::filesystem::path(); });
	sprite_type["TilingFactor"] = sol::property([](SpriteComponent& c) { return c.tilingFactor; }, [](SpriteComponent& c, const float v) { c.tilingFactor = v; });

	auto animated_sprite_type = state["AnimatedSpriteComponent"].get_or_create<sol::usertype<AnimatedSpriteComponent>>();
	animated_sprite_type["Tint"] = sol::property([](AnimatedSpriteComponent& c) -> Colour& { return c.tint; }, [](AnimatedSpriteComponent& c, const Colour& v) { c.tint = v; });
	animated_sprite_type["SpriteSheet"] = sol::property([](AnimatedSpriteComponent& c) { return c.spriteSheet; }, [](AnimatedSpriteComponent& c, const Ref<SpriteSheet>& v) { c.spriteSheet = v; });
	animated_sprite_type["Animation"] = sol::property([](AnimatedSpriteComponent& c) { return c.animation; }, [](AnimatedSpriteComponent& c, const std::string& v) { c.animation = v; });


	std::initializer_list<std::pair<sol::string_view, int>> rigidBodyTypesItems =
	{
		{ "Static", (int)RigidBody2DComponent::BodyType::STATIC },
		{ "Kinematic", (int)RigidBody2DComponent::BodyType::KINEMATIC },
		{ "Dynamic", (int)RigidBody2DComponent::BodyType::DYNAMIC }
	};
	state.new_enum("BodyType", rigidBodyTypesItems);

	auto rigidBody2D_type = state["RigidBody2DComponent"].get_or_create<sol::usertype<RigidBody2DComponent>>();
	rigidBody2D_type["Type"] = sol::property([](RigidBody2DComponent& c) { return c.type; }, [](RigidBody2DComponent& c, RigidBody2DComponent::BodyType v) { c.type = v; });
	rigidBody2D_type["FixedRotation"] = sol::property([](RigidBody2DComponent& c) { return c.fixedRotation; }, [](RigidBody2DComponent& c, bool v) { c.fixedRotation = v; });
	rigidBody2D_type["GravityScale"] = sol::property([](RigidBody2DComponent& c) { return c.gravityScale; }, [](RigidBody2DComponent& c, float v) { c.gravityScale = v; });
	rigidBody2D_type["AngularDamping"] = sol::property([](RigidBody2DComponent& c) { return c.angularDamping; }, [](RigidBody2DComponent& c, float v) { c.angularDamping = v; });
	rigidBody2D_type["LinearDamping"] = sol::property([](RigidBody2DComponent& c) { return c.linearDamping; }, [](RigidBody2DComponent& c, float v) { c.linearDamping = v; });
	rigidBody2D_type.set_function("ApplyImpulse", &RigidBody2DComponent::ApplyImpulse);
	rigidBody2D_type.set_function("ApplyImpulseAtPoint", &RigidBody2DComponent::ApplyImpulseAtPoint);
	rigidBody2D_type.set_function("ApplyForce", &RigidBody2DComponent::ApplyForce);
	rigidBody2D_type.set_function("ApplyForceAtPoint", &RigidBody2DComponent::ApplyForceAtPoint);
	rigidBody2D_type.set_function("ApplyTorque", &RigidBody2DComponent::ApplyTorque);
	rigidBody2D_type.set_function("GetLinearVelocity", &RigidBody2DComponent::GetLinearVelocity);
	rigidBody2D_type.set_function("SetLinearVelocity", &RigidBody2DComponent::SetLinearVelocity);
	rigidBody2D_type.set_function("GetAngularVelocity", &RigidBody2DComponent::GetAngularVelocity);
	rigidBody2D_type.set_function("SetAngularVelocity", &RigidBody2DComponent::SetAngularVelocity);
	rigidBody2D_type.set_function("SetTransform", &RigidBody2DComponent::SetTransform);
	rigidBody2D_type.set_function("GetTransform", [](RigidBody2DComponent& c)
		{
			Vector2f position;
			float rotation;
			c.GetTransform(position, rotation);
			return std::make_tuple(position, rotation);
		});

	auto physicsMaterial_type = state.new_usertype<PhysicsMaterial>("PhysicsMaterial");
	physicsMaterial_type.set_function("GetDensity", &PhysicsMaterial::GetDensity);
	physicsMaterial_type.set_function("SetDensity", &PhysicsMaterial::SetDensity);
	physicsMaterial_type.set_function("GetFriction", &PhysicsMaterial::GetFriction);
	physicsMaterial_type.set_function("SetFriction", &PhysicsMaterial::SetFriction);
	physicsMaterial_type.set_function("GetRestitution", &PhysicsMaterial::GetRestitution);
	physicsMaterial_type.set_function("SetRestitution", &PhysicsMaterial::SetRestitution);

	auto boxCollider2D_type = state["BoxCollider2DComponent"].get_or_create<sol::usertype<BoxCollider2DComponent>>();
	boxCollider2D_type["Offset"] = sol::property([](BoxCollider2DComponent& c) -> Vector2f& { return c.offset; }, [](BoxCollider2DComponent& c, const Vector2f& v) { c.offset = v; });
	boxCollider2D_type["Size"] = sol::property([](BoxCollider2DComponent& c) -> Vector2f& { return c.size; }, [](BoxCollider2DComponent& c, const Vector2f& v) { c.size = v; });
	boxCollider2D_type["PhysicsMaterial"] = sol::property([](BoxCollider2DComponent& c) { return c.physicsMaterial; }, [](BoxCollider2DComponent& c, const Ref<PhysicsMaterial>& v) { c.physicsMaterial = v; });

	auto circleCollider2D_type = state["CircleCollider2DComponent"].get_or_create<sol::usertype<CircleCollider2DComponent>>();
	circleCollider2D_type["Offset"] = sol::property([](CircleCollider2DComponent& c) -> Vector2f& { return c.offset; }, [](CircleCollider2DComponent& c, const Vector2f& v) { c.offset = v; });
	circleCollider2D_type["Radius"] = sol::property([](CircleCollider2DComponent& c) { return c.radius; }, [](CircleCollider2DComponent& c, float v) { c.radius = v; });
	circleCollider2D_type["PhysicsMaterial"] = sol::property([](CircleCollider2DComponent& c) { return c.physicsMaterial; }, [](CircleCollider2DComponent& c, const Ref<PhysicsMaterial>& v) { c.physicsMaterial = v; });

	auto polygonCollider2D_type = state["PolygonCollider2DComponent"].get_or_create<sol::usertype<PolygonCollider2DComponent>>();
	polygonCollider2D_type["Offset"] = sol::property([](PolygonCollider2DComponent& c) -> Vector2f& { return c.offset; }, [](PolygonCollider2DComponent& c, const Vector2f& v) { c.offset = v; });
	polygonCollider2D_type["Vertices"] = sol::property([](PolygonCollider2DComponent& c) { return c.vertices; }, [](PolygonCollider2DComponent& c, const std::vector<Vector2f>& v) { c.vertices = v; });
	polygonCollider2D_type["PhysicsMaterial"] = sol::property([](PolygonCollider2DComponent& c) { return c.physicsMaterial; }, [](PolygonCollider2DComponent& c, const Ref<PhysicsMaterial>& v) { c.physicsMaterial = v; });

	auto capsuleCollider2D_type = state["CapsuleCollider2DComponent"].get_or_create<sol::usertype<CapsuleCollider2DComponent>>();
	capsuleCollider2D_type["Offset"] = sol::property([](CapsuleCollider2DComponent& c) -> Vector2f& { return c.offset; }, [](CapsuleCollider2DComponent& c, const Vector2f& v) { c.offset = v; });
	capsuleCollider2D_type["Radius"] = sol::property([](CapsuleCollider2DComponent& c) { return c.radius; }, [](CapsuleCollider2DComponent& c, float v) { c.radius = v; });
	capsuleCollider2D_type["Height"] = sol::property([](CapsuleCollider2DComponent& c) { return c.height; }, [](CapsuleCollider2DComponent& c, float v) { c.height = v; });
	capsuleCollider2D_type["PhysicsMaterial"] = sol::property([](CapsuleCollider2DComponent& c) { return c.physicsMaterial; }, [](CapsuleCollider2DComponent& c, const Ref<PhysicsMaterial>& v) { c.physicsMaterial = v; });

	auto weldJoint2D_type = state["WeldJoint2DComponent"].get_or_create<sol::usertype<WeldJoint2DComponent>>();
	weldJoint2D_type["CollideConnected"] = sol::property([](WeldJoint2DComponent& c) { return c.collideConnected; }, [](WeldJoint2DComponent& c, bool v) { c.collideConnected = v; });
	weldJoint2D_type["Damping"] = sol::property([](WeldJoint2DComponent& c) { return c.damping; }, [](WeldJoint2DComponent& c, float v) { c.damping = v; });
	weldJoint2D_type["Stiffness"] = sol::property([](WeldJoint2DComponent& c) { return c.stiffness; }, [](WeldJoint2DComponent& c, float v) { c.stiffness = v; });

	auto circleRenderer_type = state["CircleRendererComponent"].get_or_create<sol::usertype<CircleRendererComponent>>();
	circleRenderer_type["Colour"] = sol::property([](CircleRendererComponent& c) -> Colour&{ return c.colour; }, [](CircleRendererComponent& c, const Colour& v) { c.colour = v; });
	circleRenderer_type["Radius"] = sol::property([](CircleRendererComponent& c) { return c.radius; }, [](CircleRendererComponent& c, float v) { c.radius = v; });
	circleRenderer_type["Thickness"] = sol::property([](CircleRendererComponent& c) { return c.thickness; }, [](CircleRendererComponent& c, float v) { c.thickness = v; });
	circleRenderer_type["Fade"] = sol::property([](CircleRendererComponent& c) { return c.fade; }, [](CircleRendererComponent& c, float v) { c.fade = v; });

	auto primitive_type = state["PrimitiveComponent"].get_or_create<sol::usertype<PrimitiveComponent>>();
	primitive_type["Type"] = sol::property([](PrimitiveComponent& c) { return c.type; }, [](PrimitiveComponent& c, PrimitiveComponent::Shape v) { c.type = v; });
	primitive_type["CubeWidth"] = sol::property([](PrimitiveComponent& c) { return c.cubeWidth; }, [](PrimitiveComponent& c, float v) { c.cubeWidth = v; });
	primitive_type["CubeHeight"] = sol::property([](PrimitiveComponent& c) { return c.cubeHeight; }, [](PrimitiveComponent& c, float v) { c.cubeHeight = v; });
	primitive_type["CubeDepth"] = sol::property([](PrimitiveComponent& c) { return c.cubeDepth; }, [](PrimitiveComponent& c, float v) { c.cubeDepth = v; });
	primitive_type["ShpereRadius"] = sol::property([](PrimitiveComponent& c) { return c.sphereRadius; }, [](PrimitiveComponent& c, float v) { c.sphereRadius = v; });
	primitive_type["SphereLongitudeLines"] = sol::property([](PrimitiveComponent& c) { return c.sphereLongitudeLines; }, [](PrimitiveComponent& c, uint32_t v) { c.sphereLongitudeLines = v; });
	primitive_type["SphereLatitudeLines"] = sol::property([](PrimitiveComponent& c) { return c.sphereLatitudeLines; }, [](PrimitiveComponent& c, uint32_t v) { c.sphereLatitudeLines = v; });
	primitive_type["PlaneWidth"] = sol::property([](PrimitiveComponent& c) { return c.planeWidth; }, [](PrimitiveComponent& c, float v) { c.planeWidth = v; });
	primitive_type["PlaneLength"] = sol::property([](PrimitiveComponent& c) { return c.planeLength; }, [](PrimitiveComponent& c, float v) { c.planeLength = v; });
	primitive_type["PlaneWidthLines"] = sol::property([](PrimitiveComponent& c) { return c.planeWidthLines; }, [](PrimitiveComponent& c, uint32_t v) { c.planeWidthLines = v; });
	primitive_type["PlaneLengthLines"] = sol::property([](PrimitiveComponent& c) { return c.planeLengthLines; }, [](PrimitiveComponent& c, uint32_t v) { c.planeLengthLines = v; });
	primitive_type["PlaneTileU"] = sol::property([](PrimitiveComponent& c) { return c.planeTileU; }, [](PrimitiveComponent& c, float v) { c.planeTileU = v; });
	primitive_type["PlaneTileV"] = sol::property([](PrimitiveComponent& c) { return c.planeTileV; }, [](PrimitiveComponent& c, float v) { c.planeTileV = v; });
	primitive_type["ConeBottomRadius"] = sol::property([](PrimitiveComponent& c) { return c.coneBottomRadius; }, [](PrimitiveComponent& c, float v) { c.coneBottomRadius = v; });
	primitive_type["ConeHeight"] = sol::property([](PrimitiveComponent& c) { return c.coneHeight; }, [](PrimitiveComponent& c, float v) { c.coneHeight = v; });
	primitive_type["ConeSliceCount"] = sol::property([](PrimitiveComponent& c) { return c.coneSliceCount; }, [](PrimitiveComponent& c, uint32_t v) { c.coneSliceCount = v; });
	primitive_type["ConeStackCount"] = sol::property([](PrimitiveComponent& c) { return c.coneStackCount; }, [](PrimitiveComponent& c, uint32_t v) { c.coneStackCount = v; });
	primitive_type["CylinderBottomRadius"] = sol::property([](PrimitiveComponent& c) { return c.cylinderBottomRadius; }, [](PrimitiveComponent& c, float v) { c.cylinderBottomRadius = v; });
	primitive_type["CylinderTopRadius"] = sol::property([](PrimitiveComponent& c) { return c.cylinderTopRadius; }, [](PrimitiveComponent& c, float v) { c.cylinderTopRadius = v; });
	primitive_type["CylinderHeight"] = sol::property([](PrimitiveComponent& c) { return c.cylinderHeight; }, [](PrimitiveComponent& c, float v) { c.cylinderHeight = v; });
	primitive_type["CylinderSliceCount"] = sol::property([](PrimitiveComponent& c) { return c.cylinderSliceCount; }, [](PrimitiveComponent& c, uint32_t v) { c.cylinderSliceCount = v; });
	primitive_type["CylinderStackCount"] = sol::property([](PrimitiveComponent& c) { return c.cylinderStackCount; }, [](PrimitiveComponent& c, uint32_t v) { c.cylinderStackCount = v; });
	primitive_type["TorusOuterRadius"] = sol::property([](PrimitiveComponent& c) { return c.torusOuterRadius; }, [](PrimitiveComponent& c, float v) { c.torusOuterRadius = v; });
	primitive_type["TorusInnerRadius"] = sol::property([](PrimitiveComponent& c) { return c.torusInnerRadius; }, [](PrimitiveComponent& c, float v) { c.torusInnerRadius = v; });
	primitive_type["TorusSliceCount"] = sol::property([](PrimitiveComponent& c) { return c.torusSliceCount; }, [](PrimitiveComponent& c, uint32_t v) { c.torusSliceCount = v; });
	primitive_type["Material"] = sol::property([](PrimitiveComponent& c) { return c.material; }, [](PrimitiveComponent& c, const Ref<Material>& v) { c.material = v; });
	primitive_type["Mesh"] = sol::property([](PrimitiveComponent& c) { return c.mesh; }, [](PrimitiveComponent& c, const Ref<Mesh>& v) { c.mesh = v; });
	primitive_type.set_function("SetCube", &PrimitiveComponent::SetCube);
	primitive_type.set_function("SetSphere", &PrimitiveComponent::SetSphere);
	primitive_type.set_function("SetPlane", &PrimitiveComponent::SetPlane);
	primitive_type.set_function("SetCylinder", &PrimitiveComponent::SetCylinder);
	primitive_type.set_function("SetCone", &PrimitiveComponent::SetCone);
	primitive_type.set_function("SetTorus", &PrimitiveComponent::SetTorus);
	primitive_type.set_function("SetType", &PrimitiveComponent::SetType);

	auto text_type = state["TextComponent"].get_or_create<sol::usertype<TextComponent>>();
	text_type["Text"] = sol::property([](TextComponent& c) { return c.text; }, [](TextComponent& c, const std::string& v) { c.text = v; });
	text_type["MaxWidth"] = sol::property([](TextComponent& c) { return c.maxWidth; }, [](TextComponent& c, float v) { c.maxWidth = v; });
	text_type["Colour"] = sol::property([](TextComponent& c) -> Colour& { return c.colour; }, [](TextComponent& c, const Colour& v) { c.colour = v; });
	text_type["Font"] = sol::property([](TextComponent& c) { return c.font; }, [](TextComponent& c, const Ref<Font>& v) { c.font = v; });

	auto audio_source_type = state["AudioSourceComponent"].get_or_create<sol::usertype<AudioSourceComponent>>();
	audio_source_type["Clip"] = sol::property([](AudioSourceComponent& c) { return c.audioClip; }, [](AudioSourceComponent& c, const Ref<AudioClip>& v) { c.audioClip = v; });
	audio_source_type["Volume"] = sol::property([](AudioSourceComponent& c) { return c.volume; }, [](AudioSourceComponent& c, float v) { c.volume = v; });
	audio_source_type["Pitch"] = sol::property([](AudioSourceComponent& c) { return c.pitch; }, [](AudioSourceComponent& c, float v) { c.pitch = v; });
	audio_source_type["Loop"] = sol::property([](AudioSourceComponent& c) { return c.loop; }, [](AudioSourceComponent& c, bool v) { c.loop = v; });
	audio_source_type["MinDistance"] = sol::property([](AudioSourceComponent& c) { return c.minDistance; }, [](AudioSourceComponent& c, float v) { c.minDistance = v; });
	audio_source_type["MaxDistance"] = sol::property([](AudioSourceComponent& c) { return c.maxDistance; }, [](AudioSourceComponent& c, float v) { c.maxDistance = v; });
	audio_source_type["Rolloff"] = sol::property([](AudioSourceComponent& c) { return c.rolloff; }, [](AudioSourceComponent& c, float v) { c.rolloff = v; });
	audio_source_type["Stream"] = sol::property([](AudioSourceComponent& c) { return c.stream; }, [](AudioSourceComponent& c, bool v) { c.stream = v; });
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