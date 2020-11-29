#pragma once

#include "TransformComponent.h"
#include "TagComponent.h"
#include "CameraComponent.h"
#include "SpriteComponent.h"
#include "StaticMeshComponent.h"
#include "NativeScriptComponent.h"
#include "PrimitiveComponent.h"
#include "TilemapComponent.h"

//List of components that can be serialized
#define COMPONENTS		\
TransformComponent,		\
TagComponent,			\
CameraComponent,		\
SpriteComponent,		\
NativeScriptComponent	\
