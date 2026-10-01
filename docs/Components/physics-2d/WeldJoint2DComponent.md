# Weld Joint 2D

Holds this entity's physics body fixed to its parent's body.

The joint connects this entity's body to its parent's rigid body or tilemap. With no parent, it
connects to the world. Use it to attach physics objects together, such as a weapon to a
character. A `Stiffness` of 0 holds them rigidly together, and higher values let the joint
flex like a spring.

## Scene file

Saved as a `<WeldJoint2D>` element inside the entity's `<Entity>` element. Attributes that are left out keep their default. See [Scene Files](../../scene-files.md) for how values like vectors, colours and file paths are written.

```xml
<WeldJoint2D CollideConnected="false" Damping="0" Stiffness="0"/>
```

| Name | Where | Type | Default |
| --- | --- | --- | --- |
| `CollideConnected` | attribute | bool, whether the two bodies collide with each other | `false` |
| `Damping` | attribute | float | `0` |
| `Stiffness` | attribute | float | `0` |

The entity also needs a [Rigid Body 2D](RigidBody2DComponent.md) or a collider for the joint to
attach to.

--8<-- "LuaAPI/_fragments/WeldJoint2DComponent.md"
