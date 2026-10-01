# Scene Files

A scene is saved as an XML `.scene` file inside the project folder. The editor normally writes
these, but the format is simple enough to write by hand or generate with a tool. This page covers
the overall layout, and each [component](Components/index.md) page shows that component's own
element.

A prefab is a scene file too: spawn one from a script with
`CurrentScene:InstantiateScene(LoadScene("Scenes/Crate.scene"), position)` - see [Scene](LuaAPI/Scene.md).

## Layout

```xml
<Scene EngineVersion="1">
    <Gravity X="0" Y="-9.81"/>
    <Entity Name="Player" ID="1001-1">
        <Transform>...</Transform>
        <Sprite>...</Sprite>
        <Entity Name="Sword" ID="1001-2">
            ...
        </Entity>
    </Entity>
    <Entity Name="Ground" ID="1001-3">
        ...
    </Entity>
</Scene>
```

- **`<Scene>`** is the root. `EngineVersion` is the scene format version, and the engine logs a
  warning if it doesn't match its own (currently `1`).
- **`<Gravity>`** is the 2D physics gravity, in world units per second squared.
- **`<Entity>`** has a `Name` and an `ID` (see [ID](Components/core/IDComponent.md)), and holds
  one element per component.
- **Children** are written as `<Entity>` elements nested inside their parent, in order. This
  is the only way the parent and child relationship is stored.

An entity only gets an ID and a name automatically. Add a `<Transform>` to almost every entity,
because rendering, physics, cameras and audio all need one.

Each component element appears at most once per entity, and the order of components inside an
entity doesn't matter. Elements the engine doesn't recognise are ignored. Attributes that are
left out keep the component's default value.

## Values

| Kind | Written as | Example |
| --- | --- | --- |
| Number | attribute | `Radius="0.5"` |
| Boolean | attribute, `true` or `false` | `IsTrigger="false"` |
| Enum | attribute, an integer (each component page lists the values) | `BodyType="2"` |
| 2D vector | child element with `X` and `Y` | `<Offset X="0" Y="0.5"/>` |
| 3D vector | child element with `X`, `Y` and `Z` | `<Position X="0" Y="0" Z="0"/>` |
| Colour | child element with `R`, `G`, `B`, `A` from 0 to 1 | `<Tint R="1" G="0.5" B="0" A="1"/>` |
| Asset | `Filepath` attribute, relative to the project folder, with `/` separators | `<LuaScript Filepath="Scripts/Player.lua"/>` |
| Texture | an asset, plus `FilterMethod` (`0` linear, `1` nearest) and `WrapMethod` (`0` clamp, `1` mirror, `2` repeat) | `<Texture Filepath="Sprites/Player.png" FilterMethod="1" WrapMethod="2"/>` |

Rotations are in radians.

## A complete example

A 2D scene with a camera, a player that falls under gravity onto the ground, and a pause button:

```xml
<Scene EngineVersion="1">
    <Gravity X="0" Y="-9.81"/>
    <Entity Name="Camera" ID="1-1">
        <Transform>
            <Position X="0" Y="0" Z="0"/>
            <Rotation X="0" Y="0" Z="0"/>
            <Scale X="1" Y="1" Z="1"/>
        </Transform>
        <Camera Primary="true" FixedAspectRatio="false">
            <SceneCamera ProjectionType="1" OrthoSize="10" OrthoNear="-1" OrthoFar="1"/>
        </Camera>
        <AudioListener Primary="true"/>
    </Entity>
    <Entity Name="Player" ID="1-2">
        <Transform>
            <Position X="0" Y="3" Z="0"/>
            <Rotation X="0" Y="0" Z="0"/>
            <Scale X="1" Y="1" Z="1"/>
        </Transform>
        <Sprite TilingFactor="1">
            <Texture Filepath="Sprites/Player.png" FilterMethod="1" WrapMethod="2"/>
            <Tint R="1" G="1" B="1" A="1"/>
        </Sprite>
        <RigidBody2D BodyType="2" FixedRotation="true" GravityScale="1"/>
        <CapsuleCollider2D Direction="0" Radius="0.4" Height="1" IsTrigger="false">
            <Offset X="0" Y="0"/>
        </CapsuleCollider2D>
        <LuaScript Filepath="Scripts/Player.lua"/>
    </Entity>
    <Entity Name="Ground" ID="1-3">
        <Transform>
            <Position X="0" Y="-4" Z="0"/>
            <Rotation X="0" Y="0" Z="0"/>
            <Scale X="20" Y="1" Z="1"/>
        </Transform>
        <Sprite TilingFactor="1">
            <Tint R="0.3" G="0.6" B="0.3" A="1"/>
        </Sprite>
        <BoxCollider2D IsTrigger="false">
            <Offset X="0" Y="0"/>
            <Size X="0.5" Y="0.5"/>
        </BoxCollider2D>
    </Entity>
    <Entity Name="HUD" ID="1-4">
        <Canvas PixelPerUnit="1"/>
        <Entity Name="Pause Button" ID="1-5">
            <Widget FixedWidth="true" FixedHeight="true"
                    AnchorLeft="1" AnchorRight="1" AnchorTop="0" AnchorBottom="0"
                    MarginLeft="-220" MarginRight="-20" MarginTop="20" MarginBottom="80">
                <Position X="1700" Y="20"/>
                <Size X="200" Y="60"/>
            </Widget>
            <Button>
                <NormalTint R="1" G="1" B="1" A="1"/>
                <HoveredTint R="0.9" G="0.9" B="0.9" A="1"/>
                <ClickedTint R="0.7" G="0.7" B="0.7" A="1"/>
                <DisabledTint R="0.5" G="0.5" B="0.5" A="1"/>
            </Button>
            <LuaScript Filepath="Scripts/PauseButton.lua"/>
        </Entity>
    </Entity>
</Scene>
```

The ground has no rigid body, so its collider holds still. The box collider's `Size` is half the
width and height, scaled by the transform, so 0.5 × 0.5 exactly fits the 20 × 1 sprite.
