# Widget

Position and size of a UI element, anchored to its parent.

!!! warning "Work in progress"
    In game UI is still in development

Widgets are laid out on a virtual 1920×1080 screen, and all sizes, margins and positions are
in pixels on that screen. The result is scaled to the real window size.

**Anchors** are fractions of the parent's size, from 0 to 1: `0` is the left or top edge and `1`
is the right or bottom edge. **Margins** are pixel offsets from the anchors. With
`FixedWidth`, the widget keeps its own width wherever its parent edges are. Without it, the
left and right edges each follow their own anchor, so the widget stretches. Height works the
same way with `FixedHeight` and the top and bottom anchors.

For example, a 200×60 button 20 pixels in from the bottom-right corner:
anchors all `1`, `MarginLeft="-220"` `MarginTop="-80"`, `FixedWidth` and `FixedHeight` true,
`Size` 200×60.

Inside a [Stack Layout](StackLayoutComponent.md), [Grid Layout](GridLayoutComponent.md) or
[Scroll Box](ScrollBoxComponent.md), the container positions the widget and its anchors are
ignored.

## Scene file

Saved as a `<Widget>` element inside the entity's `<Entity>` element. Attributes that are left out keep their default. See [Scene Files](../../scene-files.md) for how values like vectors, colours and file paths are written.

```xml
<Widget Disabled="false" FixedWidth="true" FixedHeight="true"
        AnchorLeft="1" AnchorRight="1" AnchorTop="1" AnchorBottom="1"
        MarginLeft="-220" MarginRight="-20" MarginTop="-80" MarginBottom="-20" Rotation="0">
    <Position X="1700" Y="1000"/>
    <Size X="200" Y="60"/>
</Widget>
```

| Name | Where | Type | Default |
| --- | --- | --- | --- |
| `Disabled` | attribute | bool, greys out a button and stops it responding | `false` |
| `FixedWidth` / `FixedHeight` | attributes | bool | `true` |
| `AnchorLeft` `AnchorRight` `AnchorTop` `AnchorBottom` | attributes | float, 0-1 | `0` |
| `MarginLeft` `MarginTop` | attributes | float, pixels | `0` |
| `MarginRight` `MarginBottom` | attributes | float, pixels | `100` |
| `Rotation` | attribute | float, radians | `0` |
| `Position` | child `X` `Y` | float, pixels; recalculated from anchors and margins | 0, 0 |
| `Size` | child `X` `Y` | float, pixels; used when `FixedWidth` / `FixedHeight` is set | 100, 100 |

--8<-- "LuaAPI/_fragments/WidgetComponent.md"
