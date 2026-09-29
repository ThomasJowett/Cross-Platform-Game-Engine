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

## Behaviour tree custom tasks

A behaviour tree's **Custom Task** node also runs a Lua script, with its own set of functions
(`OnStateEntry`, `OnStateUpdate`, `OnStateExit`) and a `Blackboard` global. See
[Behaviour Trees](behaviour-trees.md#custom-task-scripts).

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

## Pathfinding across a tilemap

`Pathfinding.FindPath(start, goal, tilemapEntity)` runs A* over an orthogonal or isometric
tilemap and returns an array of `Vec2` waypoints, the centre of each tile from `start` to `goal`
(for isometric maps, the middle of each tile's diamond). Tiles
whose tileset tile has a collision shape are treated as walls; empty tiles and tiles without
a collision shape are walkable. The grid comes straight from the tilemap, so it always matches
what's drawn, even if the tilemap entity is moved, scaled, rotated or parented.

```lua
local speed = 3.0
local path = {}
local nextPoint = 1

function OnCreate()
    local level = CurrentScene:FindEntity("Level")
    local player = CurrentScene:FindEntity("Player")

    local from = CurrentEntity:GetTransformComponent().Position
    local to = player:GetTransformComponent().Position
    path = Pathfinding.FindPath(Vec2.new(from.x, from.y), Vec2.new(to.x, to.y), level)
end

function OnUpdate(deltaTime)
    local target = path[nextPoint]
    if target == nil then return end

    local transform = CurrentEntity:GetTransformComponent()
    local toTarget = Vec3.new(target.x, target.y, transform.Position.z) - transform.Position
    local distance = toTarget:Length()
    local step = speed * deltaTime
    if distance <= step then
        transform.Position = Vec3.new(target.x, target.y, transform.Position.z)
        nextPoint = nextPoint + 1
    else
        transform.Position = transform.Position + toTarget * (step / distance)
    end
end
```

The result is empty if either end is off the tilemap or on a wall, or if no route exists.
Pass `false` as a fourth argument to limit movement to up/down/left/right; diagonal moves
never cut the corner of a wall. A tilemap marked `isTrigger` doesn't block anything.

## Where to go next

The [Lua API Reference](LuaAPI/index.md) documents every property/function currently exposed
this way, grouped by component - it's generated directly from the engine's source, so it always
reflects what's actually available. Not every component is fully documented there yet (only
ones migrated to the newer binding macros show their fields/functions); anything not listed yet
is still usable, just not documented here for the moment.
