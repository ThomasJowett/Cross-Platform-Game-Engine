# ID

The entity's unique ID, which stays the same across saves.

Every entity has one, created along with the entity. Unlike an `Entity` value in a script,
which is only valid while the scene is loaded, the ID is the same every time the scene loads, so
it's the way to refer to an entity in a save file or across a scene reload.

## Scene file

```xml
<Entity Name="Player" ID="9642493918600844417-5194653471942092966">
    ...
</Entity>
```

| Name | Where | Type | Default |
| --- | --- | --- | --- |
| `ID` | attribute on `<Entity>` | Two unsigned 64-bit integers joined by `-` | A new random ID |

When writing a scene by hand, any two different 64-bit numbers work, as long as no two
entities in the scene share an ID. If `ID` is left out, the entity gets a new random ID when the
scene loads, and the engine logs a warning.

## Using IDs in scripts

Get an entity's ID with `entity:GetID()`, and find the entity again with
`CurrentScene:FindEntityByID(id)`. Like `FindEntity`, it returns an invalid entity if there's
no match, so check the result with `IsSceneValid()`. IDs are read-only: scripts
can't create one or change an entity's ID. See [UUID](../../LuaAPI/UUID.md).

```lua
-- Remember which chest was opened
local saved = CurrentEntity:GetID():ToString()

-- Later, or after the scene reloads
local chest = CurrentScene:FindEntityByID(UUID.FromString(saved))
if chest:IsSceneValid() then
    Log.Info("Found " .. chest:GetName())
end
```

- **Table keys:** use `id:ToString()` as the key, not the ID itself. Lua tells two ID objects
  apart by identity, so `t[entity:GetID()]` set once won't be found by a later `GetID()` call.
  Comparing with `==` does compare values.
- **Spawned copies:** entities spawned with `InstantiateScene` or `InstantiateEntity` get new
  IDs, not the IDs in the prefab.

--8<-- "LuaAPI/_fragments/IDComponent.md"
