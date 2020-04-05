#pragma once

// For including in applications

#include "Core/Layer.h"
#include "Core/Application.h"

#include "Core/Input.h"
#include "Core/Joysticks.h"
#include "Core/KeyCodes.h"
#include "Core/MouseButtonCodes.h"
#include "Core/Colour.h"
#include "OrthoGrpahicCameraController.h"
#include "PerspectiveCameraController.h"

#include "Logging/Logger.h"
#include "Logging/Timer.h"
#include "Logging/Instrumentor.h"

#include "ImGui/ImGuiLayer.h"

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

#include "Utilities/GeometryGenerator.h"
#include "Utilities/Random.h"
#include "Utilities/StringUtils.h"