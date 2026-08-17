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

	// If the currently loaded scene is one of the given files, reloads it so its live
	// in-memory component data (e.g. a SpriteComponent's cached texture Ref) picks up
	// the update - otherwise a later save would silently re-serialize the stale
	// reference and undo it. No-op (with a warning) if the scene has unsaved changes,
	// so this never silently discards the user's other edits.
	void ReloadCurrentSceneIfAffected(const std::vector<std::filesystem::path>& updatedFiles);
}
