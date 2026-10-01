# Components

An entity is just an ID and a name. Everything else it does comes from the components attached to it. Add components from the **Properties** panel in the editor, from a script with `entity:Add<Name>()`, or by writing them into a scene file - see [Scene Files](../scene-files.md).

## Core

| Component | Scene element | What it does |
| --- | --- | --- |
| [ID](core/IDComponent.md) | on `<Entity>` | The entity's unique ID, which stays the same across saves. |
| [Name](core/NameComponent.md) | on `<Entity>` | The entity's display name. |
| [Transform](core/TransformComponent.md) | `<Transform>` | Position, rotation and scale. |
| [Hierarchy](core/HierarchyComponent.md) | on `<Entity>` | Links an entity to its parent and children. |
| [Hidden](core/HiddenComponent.md) | `<Hidden>` | Stops an entity, and everything under it, from being drawn. |

## Rendering

| Component | Scene element | What it does |
| --- | --- | --- |
| [Camera](rendering/CameraComponent.md) | `<Camera>` | The point of view the scene is rendered from. |
| [Sprite](rendering/SpriteComponent.md) | `<Sprite>` | Draws a textured or plain coloured quad. |
| [Animated Sprite](rendering/AnimatedSpriteComponent.md) | `<AnimatedSprite>` | Plays a named animation from a sprite sheet. |
| [Circle Renderer](rendering/CircleRendererComponent.md) | `<CircleRenderer>` | Draws a filled circle or a ring, with no texture needed. |
| [Static Mesh](rendering/StaticMeshComponent.md) | `<StaticMesh>` | Draws an imported 3D model. |
| [Primitive](rendering/PrimitiveComponent.md) | `<Primitive>` | Draws a generated 3D shape: cube, sphere, plane, cylinder, cone or torus. |
| [Tilemap](rendering/TilemapComponent.md) | `<Tilemap>` | A grid of tiles drawn from a tileset, with optional collision. |
| [Text](rendering/TextComponent.md) | `<Text>` | Draws a string, in the world or in in-game UI. |
| [Point Light](rendering/PointLightComponent.md) | `<PointLight>` | A light that shines in every direction from the entity's position. |
| [Billboard](rendering/BillboardComponent.md) | `<Billboard>` | Keeps an entity facing the camera. |

## Physics 2D

| Component | Scene element | What it does |
| --- | --- | --- |
| [Rigid Body 2D](physics-2d/RigidBody2DComponent.md) | `<RigidBody2D>` | Makes an entity a 2D physics body that can be moved by gravity, forces and collisions. |
| [Box Collider 2D](physics-2d/BoxCollider2DComponent.md) | `<BoxCollider2D>` | A rectangular collision shape. |
| [Circle Collider 2D](physics-2d/CircleCollider2DComponent.md) | `<CircleCollider2D>` | A circular collision shape. |
| [Capsule Collider 2D](physics-2d/CapsuleCollider2DComponent.md) | `<CapsuleCollider2D>` | A pill-shaped collision shape, made from two circles and a box. |
| [Polygon Collider 2D](physics-2d/PolygonCollider2DComponent.md) | `<PolygonCollider2D>` | A collision shape with any outline you like. |
| [Weld Joint 2D](physics-2d/WeldJoint2DComponent.md) | `<WeldJoint2D>` | Holds this entity's physics body fixed to its parent's body. |

## Scripting & AI

| Component | Scene element | What it does |
| --- | --- | --- |
| [Lua Script](scripting-ai/LuaScriptComponent.md) | `<LuaScript>` | Runs a Lua script on the entity. |
| [Behaviour Tree](scripting-ai/BehaviourTreeComponent.md) | `<BehaviourTree>` | Runs a behaviour tree on the entity, for AI decisions. |
| [State Machine](scripting-ai/StateMachineComponent.md) | `<StateMachine>` | Placeholder for state machine AI. |

## Audio

| Component | Scene element | What it does |
| --- | --- | --- |
| [Audio Source](audio/AudioSourceComponent.md) | `<AudioSource>` | Plays an audio clip from the entity's position. |
| [Audio Listener](audio/AudioListenerComponent.md) | `<AudioListener>` | The point that sound is heard from, usually the camera or the player. |

## In-game UI

| Component | Scene element | What it does |
| --- | --- | --- |
| [Canvas](ui/CanvasComponent.md) | `<Canvas>` | The root of a piece of in-game UI. |
| [Widget](ui/WidgetComponent.md) | `<Widget>` | Position and size of a UI element, anchored to its parent. |
| [Button](ui/ButtonComponent.md) | `<Button>` | A clickable UI element, with a different look for each state. |
| [Stack Layout](ui/StackLayoutComponent.md) | `<StackLayout>` | Arranges child widgets in a row or a column. |
| [Grid Layout](ui/GridLayoutComponent.md) | `<GridLayout>` | Arranges child widgets in a grid with a fixed number of columns. |
| [Scroll Box](ui/ScrollBoxComponent.md) | `<ScrollBox>` | Scrolls a single child widget that's bigger than the box. |
