# Polygon Collider 2D

A collision shape with any outline you like.

The vertices outline the shape in order, relative to the entity's position. Concave shapes are
fine, because the outline is split into triangles. The default is a pentagon.

## Scene file

Saved as a `<PolygonCollider2D>` element inside the entity's `<Entity>` element. Attributes that are left out keep their default. See [Scene Files](../../scene-files.md) for how values like vectors, colours and file paths are written.

```xml
<PolygonCollider2D IsTrigger="false">
    <Offset X="0" Y="0"/>
    <Vertex X="0" Y="1"/>
    <Vertex X="-1" Y="-1"/>
    <Vertex X="1" Y="-1"/>
</PolygonCollider2D>
```

| Name | Where | Type | Default |
| --- | --- | --- | --- |
| `Vertex` | child `X` `Y`, repeatable | float, one per corner, in order | a pentagon |
| `Offset` | child `X` `Y` | float, shape offset from the entity's position | 0, 0 |
| `IsTrigger` | attribute | bool, reports overlaps without blocking | `false` |
| `PhysicsMaterial` | child `Filepath`, optional | path to a `.physicsmaterial` (density, friction, restitution) | built-in defaults |

Collider sizes are multiplied by the entity's transform scale. Without a
[Rigid Body 2D](RigidBody2DComponent.md) on the same entity, the collider is attached to a body
that doesn't respond to forces or gravity, so it acts as a solid obstacle. A
[Lua Script](../scripting-ai/LuaScriptComponent.md) on the entity can define `OnBeginContact(other,
normal, point)` and `OnEndContact(other)` to hear about collisions and trigger overlaps.

The shape is built when the game starts, so changing collider properties while it's running
has no effect.

--8<-- "LuaAPI/_fragments/PolygonCollider2DComponent.md"
