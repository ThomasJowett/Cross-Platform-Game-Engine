# Lua Scripting

Attach a `Lua Script` component to an entity and point it at a `.lua` file to give that entity
behaviour. A script can define any of these lifecycle functions - all are optional:

```lua
function OnCreate()
    -- Runs once, when the entity is created / the scene starts
end

function OnUpdate(deltaTime)
    -- Runs once per rendered frame
end

function OnFixedUpdate()
    -- Runs on a fixed timestep - use this for physics-affecting logic
end

function OnDestroy()
    -- Runs once, just before the entity is destroyed
end
```

Every script has two globals available without needing to look anything up:

- `CurrentEntity` - the entity this script is attached to
- `CurrentScene` - the currently loaded scene

## Reading and writing components

Every component type `X` gets `CurrentEntity:AddX()`, `CurrentEntity:GetX()`,
`CurrentEntity:HasX()`, `CurrentEntity:GetOrAddX()` and `CurrentEntity:RemoveX()` - see the
[Entity](LuaAPI/Entity.md) page for the full list of components. For example, moving an entity
based on input:

```lua
function OnUpdate(deltaTime)
    local transform = CurrentEntity:GetTransformComponent()
    if not transform then return end

    local speed = 3.0
    if Input.IsKeyPressed('D') then
        transform.Position = transform.Position + Vec3.new(speed * deltaTime, 0, 0)
    end
    if Input.IsKeyPressed('A') then
        transform.Position = transform.Position - Vec3.new(speed * deltaTime, 0, 0)
    end
end
```

## Logging

```lua
function OnCreate()
    Log.Info("Entity created: " .. CurrentEntity:GetName())
end
```

See the [Log](LuaAPI/Log.md) page for all the available log levels.

## Spawning prefabs

A "prefab" is just a small scene file containing whatever entity/entities you want to spawn
repeatedly - use `LoadScene` to load it, then `Scene:InstantiateScene` to spawn a copy of it
into the current scene at a given position:

```lua
function OnCreate()
    local prefab = LoadScene("Scenes/Obstacle.scene")
    CurrentScene:InstantiateScene(prefab, Vec3.new(5, 0, 0))
end
```

## Changing scenes

```lua
function OnCreate()
    ChangeScene("Scenes/MainMenu.scene")
end
```

## Communicating between entities with signals

Two entities' scripts don't have a direct reference to each other - [Signal](LuaAPI/Signal.md)
is how they talk without one. `Signal.Emit` broadcasts a named signal; any entity that's called
`Signal.Connect` for that name receives it, regardless of where it is in the scene.

Here, a switch entity emits a signal when pressed, and a separate door entity reacts to it:

```lua
-- Attached to the "Switch" entity
function OnUpdate(deltaTime)
    if Input.IsKeyPressed('E') then
        local data = {}
        data.opened = true
        Signal.Emit("SwitchToggled", CurrentEntity, data)
    end
end
```

```lua
-- Attached to the "Door" entity
function OnCreate()
    Signal.Connect("SwitchToggled", CurrentEntity, function(sender, data)
        Log.Info(sender:GetName() .. " toggled the switch")

        local transform = CurrentEntity:GetTransformComponent()
        if transform then
            transform.Position = transform.Position + Vec3.new(0, 2, 0)
        end
    end)
end

-- Always disconnect what you connected, so a destroyed/disabled entity doesn't keep
-- reacting to signals after it's gone.
function OnDestroy()
    Signal.Disconnect("SwitchToggled", CurrentEntity)
end
```

The callback receives the `sender` (the entity that emitted the signal) and `data` (the table
passed to `Emit`), so you can pass along whatever information the listener needs.

## Where to go next

The [Lua API Reference](LuaAPI/index.md) documents every property/function currently exposed
this way, grouped by component - it's generated directly from the engine's source, so it always
reflects what's actually available. Not every component is fully documented there yet (only
ones migrated to the newer binding macros show their fields/functions); anything not listed yet
is still usable, just not documented here for the moment.
