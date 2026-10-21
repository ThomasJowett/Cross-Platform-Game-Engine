# Animated Sprite

Plays a named animation from a sprite sheet.

A `.spritesheet` asset holds a texture split into frames, plus named animations made from those
frames. This component plays one of the animations on a 1×1 quad, the same size as a
[Sprite](SpriteComponent.md). Change `Animation` from a script to switch between animations, for
example from "Idle" to "Run".

## Animation events

An animation in a `.spritesheet` can have named events on any of its frames. Frames are counted from 0
at the animation's own first frame. Add them in the sprite sheet viewer, under each frame in the
**Events** strip. When the animation reaches a frame with events, the entity's script gets
`OnAnimationEvent(eventName, animationName)` for each one, in order. `OnAnimationFinished(animationName)`
is called each time the animation's last frame has been shown for its full frame time.

```lua
function OnAnimationEvent(eventName, animationName)
    if eventName == "Active" then
        -- the sword is at its furthest point: apply damage now
    end
end
```

Any name can be used, and the same name can go on several frames or several animations. One frame
can't have two events with the same name. Changing `Animation` to a different animation starts it
from frame 0, so events on frame 0 fire straight away.

In the `.spritesheet` file each event is an `<Event>` inside its `<Animation>`:

```xml
<Animation Name="Attack_Down" StartFrame="64" FrameCount="8" HoldTime="0.1">
    <Event Frame="3" Name="Active"/>
</Animation>
```

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
