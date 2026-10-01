# Hierarchy

Links an entity to its parent and children.

You don't add this one yourself. The editor adds it when you drag entities onto each other in
the Hierarchy panel. Children move, rotate and scale with their parent, and hiding a parent with
[Hidden](HiddenComponent.md) hides its children too.

## Scene file

```xml
<Entity Name="Ship" ID="1-1">
    <Transform>...</Transform>
    <Entity Name="Turret" ID="1-2">
        <Transform>...</Transform>
    </Entity>
</Entity>
```

There's no `<Hierarchy>` element. Parent and child are written by nesting `<Entity>`
elements, and children keep the order they're written in. Scripts walk the hierarchy with
`entity:GetParent()`, `entity:GetChild()` and `entity:GetSibling()`, and build it with
`entity:AddChild()`.

Entities with a rigid body or a collider are detached from their parent when the game starts, so
physics can move them freely.

--8<-- "LuaAPI/_fragments/HierarchyComponent.md"
