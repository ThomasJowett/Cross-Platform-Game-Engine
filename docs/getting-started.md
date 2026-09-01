# Getting Started

This page assumes you already have the Editor - either downloaded from the
[Releases](https://github.com/ThomasJowett/Cross-Platform-Game-Engine/releases) page, or built
from source. If you're building from source instead, see the
[repo README](https://github.com/ThomasJowett/Cross-Platform-Game-Engine#getting-started) for
prerequisites and build instructions.

## Creating a project

Open the Editor. If no project is open, you'll land on the projects start screen; otherwise use
**File > New Project** (`Ctrl+Shift+N`) to create one, or **File > Open Project...** (`Ctrl+O`)
to open an existing one.

A project is a folder on disk containing a `.proj` file, your scenes, and your assets (sprites,
meshes, materials, scripts, and so on) - there's no separate "import" step that copies files
elsewhere, you work directly with the folder.

## Building a scene

Add entities via the Hierarchy panel, and attach/edit components (Sprite, Transform, Camera,
Lua Script, ...) via the Properties panel. Drag assets from the Content Explorer into the
viewport or onto a component field to assign them. See the
[Lua API Reference](LuaAPI/index.md) for scripting an entity's behaviour, and
[Lua Scripting](lua-scripting.md) for worked examples.

## Exporting a game

**File > Export Game** packages your project into a standalone, shippable executable - it
discovers every asset your scenes actually reference, lets you review what's about to be
included, and packs it all (scripts, textures, meshes, shaders, and so on) into the exported
build. The exported game runs independently of the Editor; players don't need the Editor
installed at all.

!!! tip
    Assets referenced only through a sprite atlas or a Material/Button/Tileset that also uses
    them are handled automatically - you don't need to hunt down what's actually used before
    exporting.
