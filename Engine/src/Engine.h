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
#include "Core/Asset.h"
#include "Core/Version.h"
#include "Core/BoundingBox.h"

// Logging
#include "Logging/Logger.h"
#include "Logging/Timer.h"
#include "Logging/Instrumentor.h"

// Imgui
#include "ImGui/ImGuiManager.h"
#include "ImGui/ImGuiUtilities.h"

// Renderer
#include "Renderer/Camera.h"
#include "Renderer/Renderer.h"
#include "Renderer/Renderer2D.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/Buffer.h"
#include "Renderer/GraphicsContext.h"
#include "Renderer/RendererAPI.h"
#include "Renderer/SubTexture2D.h"
#include "Renderer/Mesh.h"
#include "Renderer/FrameBuffer.h"

// Assets
#include "Asset/Shader.h"
#include "Asset/Texture.h"

// Utilities
#include "Utilities/GeometryGenerator.h"
#include "Utilities/Random.h"
#include "Utilities/StringUtils.h"
#include "Utilities/FileWatcher.h"

// Scene
#include "Scene/Scene.h"
#include "Scene/Entity.h"
#include "Scene/Components.h"
#include "Scene/SceneManager.h"
#include "Scene/SceneSerializer.h"
#include "Scene/AssetManager.h"
#include "Scene/SceneGraph.h"

// Events
#include "Events/ApplicationEvent.h"
#include "Events/Event.h"
#include "Events/JoystickEvent.h"
#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"
#include "Events/SceneEvent.h"

// Third Party
#include "EnTT/entt.hpp"
#include "imgui/imgui.h"
