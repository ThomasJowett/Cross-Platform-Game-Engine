# Stack Layout

Arranges child widgets in a row or a column.

!!! warning "Work in progress"
    In game UI is still in development

Children are placed one after another in hierarchy order, with `Spacing` between them and
padding inside the edges. With `StretchCrossAxis`, each child fills the stack's full width in a
column, or full height in a row, unless that child is fixed size on that axis. The stack's own
length grows to fit its children. Put it on an entity that also has a
[Widget](WidgetComponent.md).

## Scene file

Saved as a `<StackLayout>` element inside the entity's `<Entity>` element. Attributes that are left out keep their default. See [Scene Files](../../scene-files.md) for how values like vectors, colours and file paths are written.

```xml
<StackLayout Horizontal="false" Spacing="8" StretchCrossAxis="true"
             PaddingLeft="10" PaddingTop="10" PaddingRight="10" PaddingBottom="10"/>
```

| Name | Where | Type | Default |
| --- | --- | --- | --- |
| `Horizontal` | attribute | bool, `true` for a row, `false` for a column | `false` |
| `Spacing` | attribute | float, pixels | `0` |
| `StretchCrossAxis` | attribute | bool | `true` |
| `PaddingLeft` `PaddingTop` `PaddingRight` `PaddingBottom` | attributes | float, pixels | `0` |

Widgets are laid out on a virtual 1920×1080 screen, and all sizes, margins and positions are
in pixels on that screen. The result is scaled to the real window size.

--8<-- "LuaAPI/_fragments/StackLayoutComponent.md"
