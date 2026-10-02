# Circle Collider 2D

A circular collision shape.

The radius is multiplied by the larger of the transform's X and Y scale, so the shape stays a
circle.

## Scene file

Saved as a `<CircleCollider2D>` element inside the entity's `<Entity>` element. Attributes that are left out keep their default. See [Scene Files](../../scene-files.md) for how values like vectors, colours and file paths are written.

```xml
<CircleCollider2D Radius="0.5" IsTrigger="false" Layer="1" Mask="65535">
    <Offset X="0" Y="0"/>
</CircleCollider2D>
```

| Name | Where | Type | Default |
| --- | --- | --- | --- |
| `Radius` | attribute | float | `0.5` |
| `Offset` | child `X` `Y` | float, shape offset from the entity's position | 0, 0 |
| `IsTrigger` | attribute | bool, reports overlaps without blocking | `false` |
| `Layer` | attribute | int, bit of the [collision layer](../../collision-layers.md) the collider is on | `1` (`Default`) |
| `Mask` | attribute | int, bits of the collision layers it collides with | `65535` (everything) |
| `PhysicsMaterial` | child `Filepath`, optional | path to a `.physicsmaterial` (density, friction, restitution) | built-in defaults |

Collider sizes are multiplied by the entity's transform scale. Without a
[Rigid Body 2D](RigidBody2DComponent.md) on the same entity, the collider is attached to a body
that doesn't respond to forces or gravity, so it acts as a solid obstacle. A
[Lua Script](../scripting-ai/LuaScriptComponent.md) on the entity can define `OnBeginContact(other,
normal, point)` and `OnEndContact(other)` to hear about collisions and trigger overlaps.

The shape is built when the game starts, so changing collider properties while it's running
has no effect.

--8<-- "LuaAPI/_fragments/CircleCollider2DComponent.md"
