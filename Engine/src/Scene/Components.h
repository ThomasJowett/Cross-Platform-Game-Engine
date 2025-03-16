#pragma once

#include "Components/IDComponent.h"
#include "Components/NameComponent.h"
#include "Components/TransformComponent.h"
#include "Components/CameraComponent.h"
#include "Components/SpriteComponent.h"
#include "Components/AnimatedSpriteComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/TilemapComponent.h"
#include "Components/RigidBody2DComponent.h"
#include "Components/BoxCollider2DComponent.h"
#include "Components/CircleCollider2DComponent.h"
#include "Components/PolygonCollider2DComponent.h"
#include "Components/CapsuleCollider2DComponent.h"
#include "Components/CircleRendererComponent.h"
#include "Components/HierarchyComponent.h"
#include "Components/LuaScriptComponent.h"
#include "Components/BehaviourTreeComponent.h"
#include "Components/StateMachineComponent.h"
#include "Components/BillboardComponent.h"
#include "Components/CanvasComponent.h"
#include "Components/TextComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/AudioSourceComponent.h"

#include "Components/UIWidgets/WidgetComponent.h"
#include "Components/UIWidgets/ButtonComponent.h"

//List of components that can be serialized
#define COMPONENTS			\
IDComponent,				\
NameComponent,				\
TransformComponent,			\
CameraComponent,			\
SpriteComponent,			\
AnimatedSpriteComponent,	\
StaticMeshComponent,		\
PrimitiveComponent,			\
TilemapComponent,			\
RigidBody2DComponent,		\
BoxCollider2DComponent,		\
CircleCollider2DComponent,	\
PolygonCollider2DComponent,	\
CapsuleCollider2DComponent,	\
CircleRendererComponent,	\
HierarchyComponent,			\
LuaScriptComponent,			\
BehaviourTreeComponent,		\
StateMachineComponent,		\
BillboardComponent,			\
CanvasComponent,			\
TextComponent,				\
PointLightComponent,		\
WidgetComponent,			\
ButtonComponent,			\
AudioSourceComponent		\

