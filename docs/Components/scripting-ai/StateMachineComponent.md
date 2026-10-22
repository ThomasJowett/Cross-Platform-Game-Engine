# State Machine

Runs a state machine on the entity, switching between named states driven by Lua scripts.

Each entity gets its own copy of the state machine, with its own current state. See
[State Machines](../../state-machines.md) for how to build one.

## Scene file

Saved as a `<StateMachine>` element inside the entity's `<Entity>` element. Attributes that are left out keep their default. See [Scene Files](../../scene-files.md) for how values like vectors, colours and file paths are written.

```xml
<StateMachine Filepath="AI/Player.statemachine"/>
```

| Name | Where | Type | Default |
| --- | --- | --- | --- |
| `Filepath` | attribute: project-relative path | path to a `.statemachine` | none |

--8<-- "LuaAPI/_fragments/StateMachineComponent.md"
