# Animated Sprite

Plays a named animation from a sprite sheet.

A `.spritesheet` asset holds a texture split into frames, plus named animations made from those
frames. This component plays one of the animations on a 1×1 quad, the same size as a
[Sprite](SpriteComponent.md). Change `Animation` from a script to switch between animations, for
example from "Idle" to "Run".

## Scene file

Saved as a `<AnimatedSprite>` element inside the entity's `<Entity>` element. Attributes that are left out keep their default. See [Scene Files](../../scene-files.md) for how values like vectors, colours and file paths are written.

```xml
<AnimatedSprite Animation="Run">
    <SpriteSheet Filepath="Sprites/Player.spritesheet"/>
    <Tint R="1" G="1" B="1" A="1"/>
</AnimatedSprite>
```

| Name | Where | Type | Default |
| --- | --- | --- | --- |
| `Animation` | attribute | string, an animation name in the sprite sheet | empty |
| `SpriteSheet` | child `Filepath` | path to a `.spritesheet` | none |
| `Tint` | child `R` `G` `B` `A` | float, 0-1 | 1, 1, 1, 1 |

--8<-- "LuaAPI/_fragments/AnimatedSpriteComponent.md"
