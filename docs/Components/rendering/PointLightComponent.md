# Point Light

A light that shines in every direction from the entity's position.

!!! warning "Work in progress"
    The renderer doesn't light the scene yet. Point lights are saved and shown in the editor
    viewport, but don't change how anything is drawn.

## Scene file

Saved as a `<PointLight>` element inside the entity's `<Entity>` element. Attributes that are left out keep their default. See [Scene Files](../../scene-files.md) for how values like vectors, colours and file paths are written.

```xml
<PointLight Range="10" Attenuation="1" CastShadows="true">
    <Colour R="1" G="1" B="1" A="1"/>
</PointLight>
```

| Name | Where | Type | Default |
| --- | --- | --- | --- |
| `Range` | attribute | float, world units | `10` |
| `Attenuation` | attribute | float | `1` |
| `CastShadows` | attribute | bool | `true` |
| `Colour` | child `R` `G` `B` `A` | float, 0-1 | 1, 1, 1, 1 |

--8<-- "LuaAPI/_fragments/PointLightComponent.md"
