# Rigid Body 2D

Makes an entity a 2D physics body that can be moved by gravity, forces and collisions.

A rigid body needs at least one collider on the same entity to collide with anything:
[Box](BoxCollider2DComponent.md), [Circle](CircleCollider2DComponent.md),
[Capsule](CapsuleCollider2DComponent.md) or [Polygon](PolygonCollider2DComponent.md). When the
game starts, the body takes its position and Z rotation from the transform. From then on,
physics moves the entity.

- **Static** bodies never move, which suits floors and walls.
- **Kinematic** bodies move only when a script sets their velocity or position, and aren't
  pushed by collisions.
- **Dynamic** bodies are moved by gravity, forces and collisions.

## Scene file

Saved as a `<RigidBody2D>` element inside the entity's `<Entity>` element. Attributes that are left out keep their default. See [Scene Files](../../scene-files.md) for how values like vectors, colours and file paths are written.

```xml
<RigidBody2D BodyType="2" FixedRotation="true" GravityScale="1"
             AngularDamping="0" LinearDamping="0"/>
```

| Name | Where | Type | Default |
| --- | --- | --- | --- |
| `BodyType` | attribute | int: `0` Static, `1` Kinematic, `2` Dynamic | `0` |
| `FixedRotation` | attribute | bool | `false` |
| `GravityScale` | attribute | float, multiplier on the scene's `Gravity` | `1` |
| `AngularDamping` | attribute | float | `0` |
| `LinearDamping` | attribute | float | `0` |

`FixedRotation`, `GravityScale` and both damping values only affect Dynamic bodies. Entities
with a rigid body are detached from their parent when the game starts, so physics can move
them freely.

```lua
function OnFixedUpdate()
    local body = CurrentEntity:GetRigidBody2DComponent()
    -- Thrust upwards while space is held
    if Input.IsKeyPressed(' ') then
        body:ApplyForce(Vec2.new(0, 20))
    end
end
```

--8<-- "LuaAPI/_fragments/RigidBody2DComponent.md"
