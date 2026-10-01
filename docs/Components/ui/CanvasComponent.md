# Canvas

The root of a piece of in-game UI.

!!! warning "Work in progress"
    In game UI is still in development

Every UI widget must be a descendant of an entity with a Canvas. Widgets without one aren't
laid out or drawn. Put the Canvas on a plain entity and build the UI as its children.

## Scene file

Saved as a `<Canvas>` element inside the entity's `<Entity>` element. Attributes that are left out keep their default. See [Scene Files](../../scene-files.md) for how values like vectors, colours and file paths are written.

```xml
<Entity Name="HUD" ID="...">
    <Canvas PixelPerUnit="1"/>
    <Entity Name="Pause Button" ID="...">
        <Widget .../>
        <Button>...</Button>
    </Entity>
</Entity>
```

| Name | Where | Type | Default |
| --- | --- | --- | --- |
| `PixelPerUnit` | attribute | float | `1` |

`PixelPerUnit` is saved, but layout doesn't use it yet.

--8<-- "LuaAPI/_fragments/CanvasComponent.md"
