# Audio Listener

The point that sound is heard from, usually the camera or the player.

Volume and panning of every [Audio Source](AudioSourceComponent.md) are worked out relative to
the listener marked `Primary`.

## Scene file

Saved as a `<AudioListener>` element inside the entity's `<Entity>` element. Attributes that are left out keep their default. See [Scene Files](../../scene-files.md) for how values like vectors, colours and file paths are written.

```xml
<AudioListener Primary="true"/>
```

| Name | Where | Type | Default |
| --- | --- | --- | --- |
| `Primary` | attribute | bool | `true` |

--8<-- "LuaAPI/_fragments/AudioListenerComponent.md"
