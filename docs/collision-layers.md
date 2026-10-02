# Collision Layers

Collision layers let you choose which colliders interact with each other. Each collider has:

- a **Layer**: the one category the collider belongs to, e.g. `Player`, `Enemy` or `Pickup`.
- a **Mask**: the layers it collides with.

Two colliders only interact when **each one's mask includes the other's layer**. If either side
leaves the other out, they pass through each other and neither gets `OnBeginContact`. This
applies to triggers as well as solid colliders.

By default every collider is on the `Default` layer and its mask includes every layer, so
everything collides with everything until you change it.

## Naming layers

A project can have up to 16 layers. Name them in **Project Settings → Collision Layers**. Each
of the 16 slots is one layer: type a name into an empty slot to add a layer, or clear the name to
remove it. Slot 0 is always `Default`. Click **Save** to apply the names.

The names are saved in `Generated/CollisionLayers.layers` in the project folder, and are packed
into exported games:

```xml
<CollisionLayers>
    <Layer Index="1" Name="Player"/>
    <Layer Index="2" Name="Enemy"/>
    <Layer Index="3" Name="PlayerHitbox"/>
</CollisionLayers>
```

Scene files store a collider's layer and mask as bits, not names, so renaming a layer doesn't
change which colliders are on it.

## Setting a collider's layer and mask

Every collider ([Box](Components/physics-2d/BoxCollider2DComponent.md),
[Circle](Components/physics-2d/CircleCollider2DComponent.md),
[Capsule](Components/physics-2d/CapsuleCollider2DComponent.md),
[Polygon](Components/physics-2d/PolygonCollider2DComponent.md) and
[Tilemap](Components/rendering/TilemapComponent.md) collision) has a **Layer** dropdown and a
**Mask** checklist in the Properties panel.

For example, to make a sword hitbox that only hits enemies:

| Collider | Layer | Mask |
| --- | --- | --- |
| Player body | `Player` | `Default`, `Enemy` |
| Sword hitbox (trigger) | `PlayerHitbox` | `Enemy` |
| Enemy body | `Enemy` | `Default`, `Player`, `PlayerHitbox` |

The hitbox's `OnBeginContact` now only fires for enemies, so the script doesn't need to check
what it touched.

## Lua

Each collider component has a read-only `Layer` (the layer's name) and `Mask` (a table of layer
names), and a `CollidesWithLayer(name)` function:

```lua
function OnBeginContact(other, normal, point)
    local collider = other:GetCircleCollider2DComponent()
    if collider and collider.Layer == "Enemy" then
        -- hit an enemy
    end
end
```

Like the other collider properties, the layer and mask are applied when the game starts.
