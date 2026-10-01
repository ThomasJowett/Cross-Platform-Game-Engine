# Capsule Collider 2D

A pill-shaped collision shape, made from two circles and a box.

Capsules are good for characters, because the rounded ends slide over small steps and corners
instead of catching on them. `Height` is the total length, end to end.

## Scene file

Saved as a `<CapsuleCollider2D>` element inside the entity's `<Entity>` element. Attributes that are left out keep their default. See [Scene Files](../../scene-files.md) for how values like vectors, colours and file paths are written.

```xml
<CapsuleCollider2D Direction="0" Radius="0.5" Height="2" IsTrigger="false">
    <Offset X="0" Y="0"/>
</CapsuleCollider2D>
```

| Name | Where | Type | Default |
| --- | --- | --- | --- |
| `Direction` | attribute | int: `0` vertical, `1` horizontal | `0` |
| `Radius` | attribute | float | `0.5` |
| `Height` | attribute | float, total length | `2` |
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

--8<-- "LuaAPI/_fragments/CapsuleCollider2DComponent.md"
