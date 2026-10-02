# Box Collider 2D

A rectangular collision shape.

`Size` is the box's half-width and half-height. The default 0.5 × 0.5 exactly covers a
1×1 sprite.

## Scene file

Saved as a `<BoxCollider2D>` element inside the entity's `<Entity>` element. Attributes that are left out keep their default. See [Scene Files](../../scene-files.md) for how values like vectors, colours and file paths are written.

```xml
<BoxCollider2D IsTrigger="false" Layer="1" Mask="65535">
    <PhysicsMaterial Filepath="Physics/Bouncy.physicsmaterial"/>
    <Offset X="0" Y="0"/>
    <Size X="0.5" Y="0.5"/>
</BoxCollider2D>
```

| Name | Where | Type | Default |
| --- | --- | --- | --- |
| `Offset` | child `X` `Y` | float, shape offset from the entity's position | 0, 0 |
| `Size` | child `X` `Y` | float, half-width and half-height | 0.5, 0.5 |
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

--8<-- "LuaAPI/_fragments/BoxCollider2DComponent.md"
