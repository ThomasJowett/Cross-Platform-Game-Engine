# Scroll Box

Scrolls a single child widget that's bigger than the box.

!!! warning "Work in progress"
    In game UI is still in development

The box scrolls its first child, which is usually a [Stack Layout](StackLayoutComponent.md) or
[Grid Layout](GridLayoutComponent.md) holding the actual content. `ClipContent` hides anything
outside the box. `ScrollOffset` is clamped so the content can't scroll past its own edges. Put
it on an entity that also has a [Widget](WidgetComponent.md).

## Scene file

Saved as a `<ScrollBox>` element inside the entity's `<Entity>` element. Attributes that are left out keep their default. See [Scene Files](../../scene-files.md) for how values like vectors, colours and file paths are written.

```xml
<ScrollBox HorizontalScroll="false" VerticalScroll="true" ClipContent="true">
    <ScrollOffset X="0" Y="0"/>
</ScrollBox>
```

| Name | Where | Type | Default |
| --- | --- | --- | --- |
| `HorizontalScroll` / `VerticalScroll` | attributes | bool | `false` / `true` |
| `ClipContent` | attribute | bool | `true` |
| `ScrollOffset` | child `X` `Y` | float, pixels scrolled | 0, 0 |

Widgets are laid out on a virtual 1920×1080 screen, and all sizes, margins and positions are
in pixels on that screen. The result is scaled to the real window size.

--8<-- "LuaAPI/_fragments/ScrollBoxComponent.md"
