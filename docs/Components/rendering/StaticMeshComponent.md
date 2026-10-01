# Static Mesh

Draws an imported 3D model.

Point it at a `.staticmesh` asset, made by importing an FBX, glTF or OBJ file in the Content
Explorer. Each submesh uses the material the model came with, unless a material override
replaces it.

## Scene file

Saved as a `<StaticMesh>` element inside the entity's `<Entity>` element. Attributes that are left out keep their default. See [Scene Files](../../scene-files.md) for how values like vectors, colours and file paths are written.

```xml
<StaticMesh Filepath="Meshes/Crate.staticmesh">
    <MaterialOverride Filepath="Materials/Wood.material"/>
</StaticMesh>
```

| Name | Where | Type | Default |
| --- | --- | --- | --- |
| `Filepath` | attribute: project-relative path | path to a `.staticmesh` | none |
| `MaterialOverride` | child `Filepath`, repeatable | path to a `.material`, one per submesh in order | the model's own materials |

Overrides set to the default material aren't saved, so the overrides that are saved move up to
fill the gap. If you need a custom material on a later submesh, give every submesh before it
an override too.

--8<-- "LuaAPI/_fragments/StaticMeshComponent.md"
