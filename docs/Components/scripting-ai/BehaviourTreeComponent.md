# Behaviour Tree

Runs a behaviour tree on the entity, for AI decisions.

Each entity gets its own copy of the tree and its own blackboard. See
[Behaviour Trees](../../behaviour-trees.md) for how to build one.

## Scene file

Saved as a `<BehaviourTree>` element inside the entity's `<Entity>` element. Attributes that are left out keep their default. See [Scene Files](../../scene-files.md) for how values like vectors, colours and file paths are written.

```xml
<BehaviourTree Filepath="AI/Enemy.behaviourtree"/>
```

| Name | Where | Type | Default |
| --- | --- | --- | --- |
| `Filepath` | attribute: project-relative path | path to a `.behaviourtree` | none |

--8<-- "LuaAPI/_fragments/BehaviourTreeComponent.md"
