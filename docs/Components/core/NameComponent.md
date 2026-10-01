# Name

The entity's display name.

Every entity has one. It's what the Hierarchy panel shows, and scripts can read and change it
with `entity:GetName()` and `entity:SetName()`. Names don't have to be unique.

## Scene file

```xml
<Entity Name="Player" ID="...">
    ...
</Entity>
```

| Name | Where | Type | Default |
| --- | --- | --- | --- |
| `Name` | attribute on `<Entity>` | string | `Unnamed Entity` |

--8<-- "LuaAPI/_fragments/NameComponent.md"
