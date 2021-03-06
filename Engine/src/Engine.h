#pragma once

// For including in applications

// Core
#include "Core/Layer.h"
#include "Core/Application.h"
#include "Core/Input.h"
#include "Core/Joysticks.h"
#include "Core/KeyCodes.h"
#include "Core/MouseButtonCodes.h"
#include "Core/Colour.h"
#include "Core/Settings.h"

// Misc
#include "OrthoGrpahicCameraController.h"
#include "PerspectiveCameraController.h"

// Logging
#include "Logging/Logger.h"
#include "Logging/Timer.h"
#include "Logging/Instrumentor.h"

// Imgui
#include "ImGui/ImGuiManager.h"
#include "ImGui/ImGuiUtilites.h"

// Renderer
#include "Renderer/Camera.h"
#include "Renderer/Shader.h"
#include "Renderer/VertexArray.h"
#include "Renderer/Renderer.h"
#include "Renderer/Renderer2D.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/Buffer.h"
#include "Renderer/GraphicsContext.h"
#include "Renderer/RendererAPI.h"
#include "Renderer/Texture.h"
#include "Renderer/SubTexture2D.h"
#include "Renderer/Mesh.h"
#include "Renderer/FrameBuffer.h"

// Utilities
#include "Utilities/GeometryGenerator.h"
#include "Utilities/Random.h"
#include "Utilities/StringUtils.h"

// Scene
#include "Scene/Scene.h"
#include "Scene/Entity.h"
#include "Scene/ScriptableEntity.h"
#include "Scene/Components/Components.h"
#include "Scene/SceneManager.h"

// Third Party
#include "EnTT/entt.hpp"
#include "imgui/imgui.h"
