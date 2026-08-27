#include "AssetReferenceUtils.h"

#include "Core/Application.h"
#include "Logging/Instrumentor.h"
#include "TinyXml2/tinyxml2.h"
#include "Scene/SceneManager.h"
#include "Scene/Scene.h"
#include "Scene/AssetManager.h"
#include "Scene/Components/SpriteComponent.h"
#include "Scene/Components/AnimatedSpriteComponent.h"
#include "Scene/Components/UIWidgets/ButtonComponent.h"
#include "Utilities/FileUtils.h"
#include "Asset/Material.h"
#include "Asset/SpriteSheet.h"
#include "Asset/Tileset.h"

namespace
{
	std::string NormalisePath(const std::filesystem::path& path)
	{
		std::filesystem::path normalised = path;
		normalised.make_preferred();
		std::string pathString = normalised.string();
		std::replace(pathString.begin(), pathString.end(), '\\', '/');
		return pathString;
	}

	// Matches the "Filepath" attribute convention every asset reference uses
	// (SerializationUtils::Encode/Decode(XMLElement*, path)), regardless of
	// which element it's attached to or what kind of asset it points at.
	bool ReplaceFilepathAttributesRecursive(tinyxml2::XMLElement* element, const std::string& oldPathString, const std::string& newPathString)
	{
		bool changed = false;

		const char* attribute = element->Attribute("Filepath");
		if (attribute && oldPathString == attribute)
		{
			if (!newPathString.empty())
				element->SetAttribute("Filepath", newPathString.c_str());
			changed = true;
		}

		for (tinyxml2::XMLElement* child = element->FirstChildElement(); child; child = child->NextSiblingElement())
		{
			if (ReplaceFilepathAttributesRecursive(child, oldPathString, newPathString))
				changed = true;
		}

		return changed;
	}

	static const char* s_ScannedExtensions[] = { ".material", ".scene", ".spritesheet", ".tileset" };

	bool IsScannedExtension(const std::filesystem::path& extension)
	{
		for (const char* candidate : s_ScannedExtensions)
		{
			if (extension == candidate)
				return true;
		}
		return false;
	}
}

namespace AssetReferenceUtils
{
	std::vector<std::filesystem::path> FindReferences(const std::filesystem::path& path)
	{
		PROFILE_FUNCTION();

		std::string pathString = NormalisePath(path);
		std::vector<std::filesystem::path> references;

		std::error_code errorCode;
		for (auto& entry : std::filesystem::recursive_directory_iterator(Application::GetOpenDocumentDirectory(), errorCode))
		{
			if (!entry.is_regular_file() || !IsScannedExtension(entry.path().extension()))
				continue;

			tinyxml2::XMLDocument document;
			if (document.LoadFile(entry.path().string().c_str()) != tinyxml2::XML_SUCCESS)
				continue;

			tinyxml2::XMLElement* root = document.RootElement();
			if (root && ReplaceFilepathAttributesRecursive(root, pathString, std::string()))
				references.push_back(entry.path());
		}

		return references;
	}

	std::vector<std::filesystem::path> UpdateReferences(const std::filesystem::path& oldPath, const std::filesystem::path& newPath)
	{
		PROFILE_FUNCTION();

		std::string oldPathString = NormalisePath(oldPath);
		std::string newPathString = NormalisePath(newPath);

		std::vector<std::filesystem::path> updatedFiles;

		std::error_code errorCode;
		for (auto& entry : std::filesystem::recursive_directory_iterator(Application::GetOpenDocumentDirectory(), errorCode))
		{
			if (!entry.is_regular_file() || !IsScannedExtension(entry.path().extension()))
				continue;

			tinyxml2::XMLDocument document;
			if (document.LoadFile(entry.path().string().c_str()) != tinyxml2::XML_SUCCESS)
				continue;

			tinyxml2::XMLElement* root = document.RootElement();
			if (!root)
				continue;

			if (ReplaceFilepathAttributesRecursive(root, oldPathString, newPathString))
			{
				document.SaveFile(entry.path().string().c_str());
				updatedFiles.push_back(entry.path());
				ENGINE_INFO("Updated asset reference to {0} in {1}", newPathString, entry.path().string());
			}
		}

		return updatedFiles;
	}

	void UpdateCurrentSceneTextureReferences(const std::filesystem::path& oldPath, const std::filesystem::path& newPath)
	{
		PROFILE_FUNCTION();

		Scene* currentScene = SceneManager::CurrentScene();
		if (!currentScene)
			return;

		std::string oldPathString = NormalisePath(oldPath);

		// Resolved lazily, only once a real match is found - loading newPath as a texture up front
		// meant renaming any non-texture asset tried to load it as an image regardless.
		bool replacementResolved = false;
		Ref<Texture2D> replacement;
		auto getReplacement = [&]() -> Ref<Texture2D>
		{
			if (!replacementResolved)
			{
				replacement = newPath.empty() ? nullptr : AssetManager::GetTexture(newPath);
				replacementResolved = true;
			}
			return replacement;
		};

		auto matches = [&](const Ref<Texture2D>& texture)
		{
			return texture && NormalisePath(texture->GetFilepath()) == oldPathString;
		};

		bool changed = false;

		currentScene->GetRegistry().view<SpriteComponent>().each([&](SpriteComponent& sprite)
			{
				if (matches(sprite.texture))
				{
					sprite.texture = getReplacement();
					changed = true;
				}
			});

		currentScene->GetRegistry().view<ButtonComponent>().each([&](ButtonComponent& button)
			{
				if (matches(button.icon)) { button.icon = getReplacement(); changed = true; }
				if (matches(button.normalTexture)) { button.normalTexture = getReplacement(); changed = true; }
				if (matches(button.hoveredTexture)) { button.hoveredTexture = getReplacement(); changed = true; }
				if (matches(button.clickedTexture)) { button.clickedTexture = getReplacement(); changed = true; }
				if (matches(button.disabledTexture)) { button.disabledTexture = getReplacement(); changed = true; }
			});

		if (changed)
			currentScene->MakeDirty();
	}

	void UpdateCurrentSceneAnimationReferences(const Ref<SpriteSheet>& spriteSheet, const std::string& oldName, const std::string& newName)
	{
		PROFILE_FUNCTION();

		Scene* currentScene = SceneManager::CurrentScene();
		if (!currentScene || !spriteSheet)
			return;

		bool changed = false;

		currentScene->GetRegistry().view<AnimatedSpriteComponent>().each([&](AnimatedSpriteComponent& animatedSprite)
			{
				if (animatedSprite.spriteSheet == spriteSheet && animatedSprite.animation == oldName)
				{
					animatedSprite.animation = newName;
					changed = true;
				}
			});

		if (changed)
			currentScene->MakeDirty();
	}

	void ReloadAffectedNonSceneAssets(const std::vector<std::filesystem::path>& affectedFiles)
	{
		PROFILE_FUNCTION();

		for (const std::filesystem::path& file : affectedFiles)
		{
			std::filesystem::path relativePath = FileUtils::RelativePath(file, Application::GetOpenDocumentDirectory());
			std::filesystem::path extension = file.extension();

			if (extension == ".material")
			{
				if (Ref<Material> material = AssetManager::GetAsset<Material>(relativePath))
					material->Reload();
			}
			else if (extension == ".spritesheet")
			{
				if (Ref<SpriteSheet> spriteSheet = AssetManager::GetAsset<SpriteSheet>(relativePath))
					spriteSheet->Reload();
			}
			else if (extension == ".tileset")
			{
				if (Ref<Tileset> tileset = AssetManager::GetAsset<Tileset>(relativePath))
					tileset->Reload();
			}
		}
	}
}
