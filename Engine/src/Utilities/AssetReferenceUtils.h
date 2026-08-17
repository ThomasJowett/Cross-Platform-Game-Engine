#pragma once

#include <filesystem>
#include <vector>

// Every asset that can reference another asset by path does so via the same
// Filepath="..." XML attribute convention (SerializationUtils::Encode/Decode).
// These utilities scan every .material/.scene/.spritesheet/.tileset file under
// the project for that convention, so renaming/deleting an asset can find or
// fix up whatever else references it, regardless of asset type.
namespace AssetReferenceUtils
{
	// Every asset file under the project's open-document-directory whose
	// Filepath attribute (anywhere in its XML tree) equals `path`.
	std::vector<std::filesystem::path> FindReferences(const std::filesystem::path& path);

	// Rewrites every such reference from oldPath to newPath. Returns the
	// files that were updated.
	std::vector<std::filesystem::path> UpdateReferences(const std::filesystem::path& oldPath, const std::filesystem::path& newPath);

	// Directly repoints (or, with newPath empty, clears) any texture reference to oldPath
	// held by the currently loaded scene's live entities (SpriteComponent, ButtonComponent).
	// This works regardless of whether the scene has been saved - FindReferences/
	// UpdateReferences only see what's on disk, but a texture can be assigned to a sprite
	// in an unsaved scene, in which case the disk scan can't find or fix that reference at
	// all. Marks the scene dirty if anything changed, never touches unrelated entities/data,
	// so it's always safe to call even on a scene with other unsaved edits.
	void UpdateCurrentSceneTextureReferences(const std::filesystem::path& oldPath, const std::filesystem::path& newPath = {});

	// Reloads any already-cached Material/SpriteSheet/Tileset among the given files (as
	// returned by FindReferences/UpdateReferences), so a live in-memory texture reference
	// (e.g. a Material's texture slot) picks up whatever was just rewritten - or, for a
	// straight delete, now-missing - on disk. Materials/SpriteSheets/Tilesets are always
	// backed by the AssetManager cache (unlike a scene's live entity data, which can diverge
	// from disk), so re-reading from disk is always correct here. Scenes are excluded - see
	// UpdateCurrentSceneTextureReferences for those.
	void ReloadAffectedNonSceneAssets(const std::vector<std::filesystem::path>& affectedFiles);
}
