# Primitive

Draws a generated 3D shape: cube, sphere, plane, cylinder, cone or torus.

Only the settings for the chosen `Type` are saved, in a child element named after the shape.
The element for that shape has to be there, otherwise the component isn't added when the scene
loads.

## Scene file

Saved as a `<Primitive>` element inside the entity's `<Entity>` element. Attributes that are left out keep their default. See [Scene Files](../../scene-files.md) for how values like vectors, colours and file paths are written.

```xml
<Primitive Type="0">
    <Material Filepath="Materials/Test.material"/>
    <Cube Width="1" Height="1" Depth="1"/>
</Primitive>
```

| Name | Where | Type | Default |
| --- | --- | --- | --- |
| `Type` | attribute | int: `0` Cube, `1` Sphere, `2` Plane, `3` Cylinder, `4` Cone, `5` Torus | `0` |
| `Material` | child `Filepath`, optional | path to a `.material` | default material |
| `Cube` | child | `Width` `Height` `Depth` (float) | 1, 1, 1 |
| `Sphere` | child | `Radius` (float), `LongitudeLines` `LatitudeLines` (int) | 0.5, 16, 32 |
| `Plane` | child | `Width` `Length` `TileU` `TileV` (float), `WidthLines` `LengthLines` (int) | 1, 1, 1, 1, 2, 2 |
| `Cylinder` | child | `BottomRadius` `TopRadius` `Height` (float), `SliceCount` `StackCount` (int) | 0.5, 0.5, 1, 32, 5 |
| `Cone` | child | `BottomRadius` `Height` (float), `SliceCount` `StackCount` (int) | 0.5, 1, 32, 5 |
| `Torus` | child | `OuterRadius` `InnerRadius` (float), `SliceCount` (int) | 1, 0.4, 32 |

In Lua, changing a size property such as `CubeWidth` doesn't rebuild the mesh. Call the matching
function instead, for example `primitive:SetCube(width, depth, height)`.

--8<-- "LuaAPI/_fragments/PrimitiveComponent.md"
