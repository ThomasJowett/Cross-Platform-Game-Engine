# Transform

Position, rotation and scale.

Almost every entity needs one. Rendering, physics, audio and cameras all read the entity's
position from it. It isn't added automatically, though, so an entity written by hand without
a `<Transform>` has no position at all.

Values are relative to the parent entity, or to the world for an entity with no parent. 2D
games use X and Y for position and Z for rotation.

## Scene file

Saved as a `<Transform>` element inside the entity's `<Entity>` element. Attributes that are left out keep their default. See [Scene Files](../../scene-files.md) for how values like vectors, colours and file paths are written.

```xml
<Transform>
    <Position X="0" Y="2" Z="0"/>
    <Rotation X="0" Y="0" Z="0.785"/>
    <Scale X="1" Y="1" Z="1"/>
</Transform>
```

| Name | Where | Type | Default |
| --- | --- | --- | --- |
| `Position` | child `X` `Y` `Z` | float | 0, 0, 0 |
| `Rotation` | child `X` `Y` `Z` | float, Euler angles in radians | 0, 0, 0 |
| `Scale` | child `X` `Y` `Z` | float | 1, 1, 1 |

Physics bodies take their starting position and Z rotation from the transform. While the
game runs, an entity with a [Rigid Body 2D](../physics-2d/RigidBody2DComponent.md) or a collider is
moved by the physics engine. Use the rigid body's functions to move it rather than setting
`Position` directly.

--8<-- "LuaAPI/_fragments/TransformComponent.md"
