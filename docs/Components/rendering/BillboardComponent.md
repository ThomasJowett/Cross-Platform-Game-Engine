# Billboard

Keeps an entity facing the camera.

Use it for sprites and text in a 3D scene that should always face the viewer. With
`Position` set to Camera, the entity is pinned to a point on screen instead of a point in the
world.

## Scene file

Saved as a `<Billboard>` element inside the entity's `<Entity>` element. Attributes that are left out keep their default. See [Scene Files](../../scene-files.md) for how values like vectors, colours and file paths are written.

```xml
<Billboard Orientation="1" Position="1">
    <ScreenPosition X="0" Y="0"/>
</Billboard>
```

| Name | Where | Type | Default |
| --- | --- | --- | --- |
| `Orientation` | attribute | int: `0` stays upright (world up), `1` faces the camera fully | `1` |
| `Position` | attribute | int: `0` world position, `1` fixed on screen | `0` |
| `ScreenPosition` | child `X` `Y` | float; only written and read when `Position` is `1` | 0, 0 |

--8<-- "LuaAPI/_fragments/BillboardComponent.md"
