<p align="center">
  <img width="128" height="128" src="assets/logo.png" alt="Engine logo">
</p>

# Cross-Platform Game Engine

A cross-platform game engine written in C++, supporting OpenGL and WebGPU rendering backends.

## Features

- Cross-platform rendering, with both OpenGL and WebGPU backends
- Entity-component system (ECS) based scene model, backed by [EnTT](https://github.com/skypjack/entt)
- Editor with a content browser, scene viewport, and asset pipeline (textures, meshes, materials, tilesets, audio)
- 2D and 3D rendering, including sprites, text (MSDF font atlases), and static meshes
- Built-in 2D physics via Box2D
- Lua scripting support
- Scene and asset serialization

## Where to go next

- [Getting Started](getting-started.md) - creating a project and exporting a game with the Editor
- [Lua Scripting](lua-scripting.md) - worked examples for giving entities behaviour
- [Lua API Reference](LuaAPI/index.md) - the full scripting API, generated directly from the engine's source
