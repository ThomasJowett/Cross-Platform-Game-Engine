# Grid Layout

Arranges child widgets in a grid with a fixed number of columns.

!!! warning "Work in progress"
    In game UI is still in development

Children fill the grid left to right, then top to bottom. Column width is worked out from the
grid's own width. Each row is as tall as its tallest child, or exactly `FixedRowHeight` if
that's above 0. With `UniformCellSize`, every child is resized to fill its cell, unless it's
fixed size. Put it on an entity that also has a [Widget](WidgetComponent.md).

## Scene file

Saved as a `<GridLayout>` element inside the entity's `<Entity>` element. Attributes that are left out keep their default. See [Scene Files](../../scene-files.md) for how values like vectors, colours and file paths are written.

```xml
<GridLayout Columns="3" UniformCellSize="true" FixedRowHeight="0"
            PaddingLeft="0" PaddingTop="0" PaddingRight="0" PaddingBottom="0">
    <CellSpacing X="8" Y="8"/>
</GridLayout>
```

| Name | Where | Type | Default |
| --- | --- | --- | --- |
| `Columns` | attribute | int, at least 1 | `2` |
| `CellSpacing` | child `X` `Y` | float, pixels between columns and rows | 0, 0 |
| `UniformCellSize` | attribute | bool | `true` |
| `FixedRowHeight` | attribute | float, pixels; `0` sizes rows to their content | `0` |
| `PaddingLeft` `PaddingTop` `PaddingRight` `PaddingBottom` | attributes | float, pixels | `0` |

Widgets are laid out on a virtual 1920×1080 screen, and all sizes, margins and positions are
in pixels on that screen. The result is scaled to the real window size.

--8<-- "LuaAPI/_fragments/GridLayoutComponent.md"
