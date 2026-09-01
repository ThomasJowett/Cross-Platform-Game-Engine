<h1 align="center" style="border-bottom: none;">
 <a href="https://github.com/ThomasJowett/Cross-Platform-Game-Engine/">Cross Platform Game Engine</a>
</h1>
<p align="center">
 <img width="128" height="128" src="Editor/data/Icons/Logo.png">
</p>

<h3 align="center">Cross-Platform Game Engine written in C++ that supports OpenGL and WebGPU.</h3>

<div align="center">
 <a href="https://opensource.org/licenses/MIT"><img src="https://img.shields.io/badge/License-MIT-yellow.svg" alt="License: MIT"></a>
  <a href="https://github.com/ThomasJowett/Cross-Platform-Game-Engine/actions/workflows/cmake-multi-platform.yml">
    <img src="https://github.com/ThomasJowett/Cross-Platform-Game-Engine/actions/workflows/cmake-multi-platform.yml/badge.svg?branch=master" alt="Build Status">
  </a>
 <br>                                               
 <img src="Resources/Linux Screenshot.png">
</div>

## Features

* Cross-platform rendering backend, supporting both OpenGL and WebGPU
* Entity-component system (ECS) based scene model, backed by [Entt](https://github.com/skypjack/entt)
* Editor with a content browser, scene viewport, and asset pipeline (textures, meshes, materials, tilesets, audio)
* 2D and 3D rendering, including sprites, text (MSDF font atlases), and static meshes
* Built-in 2D physics via LiquidFun/Box2D
* Lua scripting support
* Scene and asset serialization

## Getting Started

### Prerequisites
Ensure you have these installed:
* A **C++17** compiler (MSVC, Clang, or GCC)
* **CMake** 3.10 or newer

> ⚠️ **Warning**: This repository uses Git submodules. When cloning, make sure to use the `--recursive` flag:
> ```bash
> git clone --recursive https://github.com/ThomasJowett/Cross-Platform-Game-Engine
> ```
> If you've already cloned without `--recursive`, initialize the submodules afterwards with:
> ```bash
> git submodule update --init --recursive
> ```

### Building

**Windows**: Use CMake to generate Visual Studio project files, then build the `Editor` or `Runtime` target.

**Mac**: Use CMake to generate Xcode project files or Makefiles.

**Linux**: First install dependencies:
```bash
sudo apt-get install -y libx11-dev libxrandr-dev libxi-dev libxinerama-dev libgl1-mesa-dev libxcursor-dev mesa-common-dev g++ make
```
Then use CMake to generate Makefiles and build with `make`.

### Running
Once built, run the `Editor` executable to open the editor, or `Runtime` to launch a project standalone.

## Example Projects
If you would like to check out some example projects they can be downloaded from [here](https://github.com/ThomasJowett/ExampleGameProjects)
## Dependencies
* [imgui](https://github.com/ocornut/imgui) : Dear ImGui: Bloat-free Immediate Mode Graphical User interface for C++ with minimal dependencies.
* [Entt](https://github.com/skypjack/entt) : Fast and reliable entity-component system (ECS) 
* [glfw](https://github.com/glfw/glfw) : A multi-platform library for OpenGL, OpenGL ES, Vulkan, window and input.
* [spdlog](https://github.com/gabime/spdlog) : Fast C++ logging library.
* [stb](https://github.com/nothings/stb) : Single-file public domain (or MIT licensed) libraries for C/C++.
* [simpleini](https://github.com/brofield/simpleini) : Simple initialization file parser and writer.
* [glad](https://github.com/Dav1dde/glad) : Meta loader for OpenGL API.
* [OpenFBX](https://github.com/nem0/OpenFBX) : Lightweight FBX Importer.
* [cereal](https://github.com/USCiLab/cereal) : A C++11 library for serialization.
* [tinyxml2](https://github.com/leethomason/tinyxml2) : A simple, small, efficient, C++ XML parser.
* [LiquidFun](https://github.com/google/liquidfun) : 2D Physics library and Extension of Box2D that provides particle and fluid dynamics.
* [lua](https://github.com/lua/lua) : A powerful, efficient, lightweight, embeddable scripting language.
* [sol2](https://github.com/ThePhD/sol2) : A C++ single header library binding to Lua.
* [msdf atlas](https://github.com/Chlumsky/msdf-atlas-gen) : A utility for generating compact font atlases.
* [miniaudio](https://github.com/mackron/miniaudio.git) : Audio playback and capture library
