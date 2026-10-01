# Tilemap

A grid of tiles drawn from a tileset, with optional collision.

A tilemap draws a grid of tiles from a `.tileset` asset, and builds 2D physics collision from
each tile's collision shape. It can lay tiles out orthogonally, isometrically, staggered or as
hexagons. Tilemaps can be imported from Tiled, and
[Pathfinding](../../LuaAPI/Pathfinding.md) can search across them.

## Scene file

Saved as a `<Tilemap>` element inside the entity's `<Entity>` element. Attributes that are left out keep their default. See [Scene Files](../../scene-files.md) for how values like vectors, colours and file paths are written.

```xml
<Tilemap Filepath="Tilesets/Dungeon.tileset" TilesWide="4" TilesHigh="3"
         TileWidth="16" TileHeight="16" Orientation="Orthogonal" IsTrigger="false">
    <Tint R="1" G="1" B="1" A="1"/>
1,2,2,3,
4,0,0,6,
7,8,8,9
</Tilemap>
```

| Name | Where | Type | Default |
| --- | --- | --- | --- |
| `Filepath` | attribute: project-relative path | path to a `.tileset` | none |
| `TilesWide` / `TilesHigh` | attributes | int, grid size in tiles |  |
| `TileWidth` / `TileHeight` | attributes | int, tile size in pixels |  |
| `Orientation` | attribute | `Orthogonal`, `Isometric`, `Staggered` or `Hexagonal` | `Orthogonal` |
| `IsTrigger` | attribute | bool, collision reports overlaps without blocking | `false` |
| `Tint` | child `R` `G` `B` `A` | float, 0-1 | 1, 1, 1, 1 |
| text content | element text | comma-separated tile indices, top row first | all `0` |

Tile index `0` is an empty cell, and index `n` is tile `n - 1` in the tileset. The text must
contain exactly `TilesWide × TilesHigh` numbers. Line breaks are only there for readability.

--8<-- "LuaAPI/_fragments/TilemapComponent.md"
