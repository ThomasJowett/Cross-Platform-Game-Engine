#pragma once

#include "IDComponent.h"
#include "NameComponent.h"
#include "TransformComponent.h"
#include "CameraComponent.h"
#include "SpriteComponent.h"
#include "AnimatedSpriteComponent.h"
#include "StaticMeshComponent.h"
#include "PrimitiveComponent.h"
#include "TilemapComponent.h"
#include "RigidBody2DComponent.h"
#include "BoxCollider2DComponent.h"
#include "CircleCollider2DComponent.h"
#include "PolygonCollider2DComponent.h"
#include "CircleRendererComponent.h"
#include "HierarchyComponent.h"
#include "LuaScriptComponent.h"
#include "BehaviourTreeComponent.h"
#include "StateMachineComponent.h"
#include "BillboardComponent.h"
#include "CanvasComponent.h"
#include "TextComponent.h"

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
CircleRendererComponent,	\
HierarchyComponent,			\
LuaScriptComponent,			\
BehaviourTreeComponent,		\
StateMachineComponent,		\
BillboardComponent,			\
CanvasComponent,			\
TextComponent				\

