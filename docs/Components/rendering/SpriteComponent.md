# Sprite

Draws a textured or plain coloured quad.

The quad is 1×1 world units, sized by the entity's transform scale. With no texture it draws as
a solid block of `Tint`.

## Scene file

Saved as a `<Sprite>` element inside the entity's `<Entity>` element. Attributes that are left out keep their default. See [Scene Files](../../scene-files.md) for how values like vectors, colours and file paths are written.

```xml
<Sprite TilingFactor="1">
    <Texture Filepath="Sprites/Player.png" FilterMethod="1" WrapMethod="2"/>
    <Tint R="1" G="1" B="1" A="1"/>
</Sprite>
```

| Name | Where | Type | Default |
| --- | --- | --- | --- |
| `TilingFactor` | attribute | float, how many times the texture repeats | `1` |
| `Texture` | child, optional | texture: `Filepath`, `FilterMethod`, `WrapMethod` | none |
| `Tint` | child `R` `G` `B` `A` | float, 0-1 | 1, 1, 1, 1 |

--8<-- "LuaAPI/_fragments/SpriteComponent.md"
