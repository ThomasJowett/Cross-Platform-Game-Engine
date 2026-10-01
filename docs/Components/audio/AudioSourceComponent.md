# Audio Source

Plays an audio clip from the entity's position.

The clip gets quieter as the [Audio Listener](AudioListenerComponent.md) moves away from it.
It's at full volume within `MinDistance`, and stops getting quieter beyond `MaxDistance`. Set
`Stream` for long clips such as music, so they're read from disk as they play instead of being
decoded all at once.

## Scene file

Saved as a `<AudioSource>` element inside the entity's `<Entity>` element. Attributes that are left out keep their default. See [Scene Files](../../scene-files.md) for how values like vectors, colours and file paths are written.

```xml
<AudioSource Filepath="Audio/Music.mp3" Volume="0.8" Pitch="1" Loop="true"
             MinDistance="1" MaxDistance="10" Rolloff="1" Stream="true" PlayOnStart="true"/>
```

| Name | Where | Type | Default |
| --- | --- | --- | --- |
| `Filepath` | attribute: project-relative path | path to an audio file | none |
| `Volume` | attribute | float | `1` |
| `Pitch` | attribute | float | `1` |
| `Loop` | attribute | bool | `false` |
| `MinDistance` / `MaxDistance` | attributes | float, world units | `1` / `10` |
| `Rolloff` | attribute | float | `1` |
| `Stream` | attribute | bool | `false` |
| `PlayOnStart` | attribute | bool, start playing when the game starts | `false` |

--8<-- "LuaAPI/_fragments/AudioSourceComponent.md"
