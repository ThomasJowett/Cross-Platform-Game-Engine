#pragma once

#include "IDComponent.h"
#include "TagComponent.h"
#include "TransformComponent.h"
#include "CameraComponent.h"
#include "SpriteComponent.h"
#include "StaticMeshComponent.h"
#include "NativeScriptComponent.h"
#include "PrimitiveComponent.h"
#include "TilemapComponent.h"
#include "RigidBody2DComponent.h"
#include "BoxCollider2DComponent.h"
#include "CircleCollider2DComponent.h"
#include "CircleRendererComponent.h"

//List of components that can be serialized
#define COMPONENTS			\
IDComponent,				\
TagComponent,				\
TransformComponent,			\
CameraComponent,			\
SpriteComponent,			\
StaticMeshComponent,		\
NativeScriptComponent,		\
PrimitiveComponent,			\
TilemapComponent,			\
RigidBody2DComponent,		\
BoxCollider2DComponent,		\
CircleCollider2DComponent,	\
CircleRendererComponent		\

