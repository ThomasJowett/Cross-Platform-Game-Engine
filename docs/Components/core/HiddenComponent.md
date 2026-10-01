# Hidden

Stops an entity, and everything under it, from being drawn.

A hidden parent hides its children too, without changing the children's own `Hidden`
value. Hidden entities still update, run scripts and take part in physics.

## Scene file

Saved as a `<Hidden>` element inside the entity's `<Entity>` element. Attributes that are left out keep their default. See [Scene Files](../../scene-files.md) for how values like vectors, colours and file paths are written.

```xml
<Hidden Hidden="true"/>
```

| Name | Where | Type | Default |
| --- | --- | --- | --- |
| `Hidden` | attribute | bool | `true` |

--8<-- "LuaAPI/_fragments/HiddenComponent.md"
