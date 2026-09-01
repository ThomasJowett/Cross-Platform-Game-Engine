# Contributing to Cross-Platform Game Engine

This document provides foundational mandates and architectural guidance for working on the Cross-Platform Game Engine. Adhere to these instructions to ensure consistency and maintainability across the codebase.

## 🚀 Project Overview
- **Language**: C++ 17
- **Build System**: CMake
- **Platforms**: Windows, macOS, Linux
- **Renderer**: Dual-API support for OpenGL and WebGPU.
- **ECS**: EnTT (Entity Component System).
- **Scripting**: Lua integrated via Sol2.

## 🛠 Coding Standards & Conventions

### Memory Management
- **NEVER** use `std::shared_ptr` or `std::unique_ptr` directly for high-level engine objects.
- **ALWAYS** use the following custom aliases defined in `core.h`:
  - `Scope<T>` instead of `std::unique_ptr<T>`.
  - `Ref<T>` instead of `std::shared_ptr<T>`.
- Use the helper functions for instantiation:
  - `CreateScope<T>(...)`
  - `CreateRef<T>(...)`

### Formatting
- **Indentation**: Use **Tabs** for all C++ files.
- **PCH**: While the project has `stdafx.h`, it is **NOT** mandatory to include it first in every `.cpp` file.

### Naming Conventions
- **Files**: PascalCase (e.g., `Application.cpp`, `RendererAPI.h`).
- **Classes & Methods**: PascalCase (e.g., `class Scene`, `void OnUpdate()`).
- **Variables**: camelCase (e.g., `float deltaTime`).
- **Member Variables**: Prefix with `m_` (e.g., `m_IsRunning`).
- **Static Variables**: Prefix with `s_` (e.g., `s_Instance`).

### Logging
Use the provided spdlog-based macros for all logging:
- **Engine-side**: `ENGINE_TRACE`, `ENGINE_DEBUG`, `ENGINE_INFO`, `ENGINE_WARN`, `ENGINE_ERROR`, `ENGINE_CRITICAL`.
- **Client/Application-side**: `CLIENT_TRACE`, `CLIENT_DEBUG`, `CLIENT_INFO`, `CLIENT_WARN`, `CLIENT_ERROR`, `CLIENT_CRITICAL`.

### Error Handling & Assertions
- Use `CORE_ASSERT(condition, message)` for engine-level assertions.
- Use `ASSERT(condition, message)` for client-level assertions.
- For fatal crashes, use `ENGINE_CRITICAL` followed by `DEBUGBREAK()`.

### Events
- The engine uses a custom Event system.
- Use `BIND_EVENT_FN(method)` macro to bind member functions to event callbacks.

## 🏗 Architecture Details

### Application & Layers
- The `Application` class manages a `LayerStack`.
- Functionality should be encapsulated in `Layer` or `Overlay` subclasses.
- Common entry points:
  - `Editor`: Main workspace for development.
  - `Runtime`: Minimal wrapper for running shipped games.

### Rendering
- Abstracted through `RendererAPI`.
- Platform-specific implementations reside in `Engine/src/Platform/OpenGL` and `Engine/src/Platform/WebGPU`.
- Use `Renderer2D` for sprite, text, and primitive rendering.

### Scripting
- Lua bindings are managed in `Engine/src/Scripting/Lua/LuaBindings.cpp`.
- Components registered in `EnTT` should also be registered in Lua if they need to be accessible from scripts.

## 📁 Directory Structure
- `Engine/`: Core engine source and vendor libraries.
- `Editor/`: Editor-specific logic and UI (ImGui).
- `Runtime/`: Standalone game runtime.
- `Resources/`: Assets, shaders, and configurations.

## 🔧 Build Instructions
- Use CMake to generate project files for your preferred IDE or build tool.
- **New Files**: Always add new `.cpp` and `.h` files to the relevant `CMakeLists.txt`, most files are automatically added with cmake glob.
- Ensure submodules are initialized: `git submodule update --init --recursive`.
- Define `DEBUG` or `RELEASE` configurations appropriately.
