# Lua Script

Runs a Lua script on the entity.

The script can define any of these functions, and the engine calls the ones it finds:

| Function | Called |
| --- | --- |
| `OnCreate()` | once, when the game starts or the entity is spawned |
| `OnUpdate(deltaTime)` | every frame |
| `OnFixedUpdate()` | every fixed physics step (100 times a second by default) |
| `OnDestroy()` | when the entity is destroyed or the game stops |
| `OnDebugRender()` | every frame, for `Debug.Draw*` calls |
| `OnBeginContact(other, normal, point)` / `OnEndContact(other)` | when a collider on this entity starts or stops touching another |
| `OnPressed()` / `OnReleased()` / `OnHovered()` / `OnUnHovered()` | on a UI [Button](../ui/ButtonComponent.md) |
| `OnInputAction(actionName, phase)` | when an input action from the project's input mappings fires |

`CurrentEntity` inside the script is the entity it's attached to. See
[Lua Scripting](../../lua-scripting.md) for worked examples.

## Scene file

Saved as a `<LuaScript>` element inside the entity's `<Entity>` element. Attributes that are left out keep their default. See [Scene Files](../../scene-files.md) for how values like vectors, colours and file paths are written.

```xml
<LuaScript Filepath="Scripts/Player.lua"/>
```

| Name | Where | Type | Default |
| --- | --- | --- | --- |
| `Filepath` | attribute: project-relative path | path to a `.lua` file | none |

--8<-- "LuaAPI/_fragments/LuaScriptComponent.md"
