# Text

Draws a string, in the world or in in-game UI.

Text is drawn with MSDF fonts, so it stays sharp at any size. Lines wrap at `MaxWidth`. With no
`Font` set, the engine's default font is used.

## Scene file

Saved as a `<Text>` element inside the entity's `<Entity>` element. Attributes that are left out keep their default. See [Scene Files](../../scene-files.md) for how values like vectors, colours and file paths are written.

```xml
<Text MaxWidth="10">Hello world<Font Filepath="Fonts/Manrope.ttf"/>
    <Colour R="1" G="1" B="1" A="1"/>
</Text>
```

| Name | Where | Type | Default |
| --- | --- | --- | --- |
| text content | element text | the string to draw | empty |
| `MaxWidth` | attribute | float, world units | `10` |
| `Font` | child `Filepath`, optional | path to a font file | default font |
| `Colour` | child `R` `G` `B` `A` | float, 0-1 | 1, 1, 1, 1 |

Put the string first in the element, before any child elements. Only the text before the first
child element is read.

--8<-- "LuaAPI/_fragments/TextComponent.md"
