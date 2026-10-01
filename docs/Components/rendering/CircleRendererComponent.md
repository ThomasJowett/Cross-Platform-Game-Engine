# Circle Renderer

Draws a filled circle or a ring, with no texture needed.

`Thickness` 1 gives a solid disc, and smaller values cut a hole out of the middle to make a
ring. `Fade` softens the edge.

## Scene file

Saved as a `<CircleRenderer>` element inside the entity's `<Entity>` element. Attributes that are left out keep their default. See [Scene Files](../../scene-files.md) for how values like vectors, colours and file paths are written.

```xml
<CircleRenderer Radius="0.5" Thickness="1" Fade="0.005">
    <Colour R="1" G="0" B="0" A="1"/>
</CircleRenderer>
```

| Name | Where | Type | Default |
| --- | --- | --- | --- |
| `Radius` | attribute | float, world units | `0.5` |
| `Thickness` | attribute | float, 0-1 | `1` |
| `Fade` | attribute | float | `0.005` |
| `Colour` | child `R` `G` `B` `A` | float, 0-1 | 1, 1, 1, 1 |

--8<-- "LuaAPI/_fragments/CircleRendererComponent.md"
