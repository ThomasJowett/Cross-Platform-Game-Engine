# Camera

The point of view the scene is rendered from.

At runtime the scene is drawn from the camera marked `Primary`. A camera is either
orthographic, which suits 2D games, or perspective, which suits 3D. The camera looks along the
entity's transform.

## Scene file

Saved as a `<Camera>` element inside the entity's `<Entity>` element. Attributes that are left out keep their default. See [Scene Files](../../scene-files.md) for how values like vectors, colours and file paths are written.

```xml
<Camera Primary="true" FixedAspectRatio="false">
    <SceneCamera ProjectionType="1" OrthoSize="10" OrthoNear="-1" OrthoFar="1"
                 PerspectiveNear="1" PerspectiveFar="1000" FOV="1.5707964"/>
</Camera>
```

| Name | Where | Type | Default |
| --- | --- | --- | --- |
| `Primary` | attribute | bool | `true` |
| `FixedAspectRatio` | attribute | bool | `false` |
| `SceneCamera` | child | projection settings, below |  |
| `ProjectionType` | attribute on `SceneCamera` | int: `0` perspective, `1` orthographic | `0` |
| `OrthoSize` | attribute on `SceneCamera` | float, visible height in world units | `10` |
| `OrthoNear` / `OrthoFar` | attributes on `SceneCamera` | float | `-1` / `1` |
| `PerspectiveNear` / `PerspectiveFar` | attributes on `SceneCamera` | float | `1` / `1000` |
| `FOV` | attribute on `SceneCamera` | float, vertical field of view in radians | `1.5707964` (90°) |
| `AspectRatio` | attribute on `SceneCamera` | float, width / height; only read when `FixedAspectRatio` is true | `1.7777778` |

In Lua, `camera.Camera` returns a copy of the projection settings. Change the copy and then
assign it back:

```lua
local cameraComp = CurrentEntity:GetCameraComponent()
local camera = cameraComp.Camera
camera:SetOrthoSize(20)
cameraComp.Camera = camera
```

See [Camera](../../LuaAPI/Camera.md) for its functions.

--8<-- "LuaAPI/_fragments/CameraComponent.md"
