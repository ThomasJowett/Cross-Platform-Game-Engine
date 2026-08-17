#include "AssetReferenceUtils.h"

#include "Core/Application.h"
#include "Logging/Instrumentor.h"
#include "TinyXml2/tinyxml2.h"
#include "Scene/SceneManager.h"
#include "Scene/Scene.h"

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

	void ReloadCurrentSceneIfAffected(const std::vector<std::filesystem::path>& updatedFiles)
	{
		PROFILE_FUNCTION();

		Scene* currentScene = SceneManager::CurrentScene();
		if (!currentScene || currentScene->GetFilepath().empty())
			return;

		std::filesystem::path currentSceneAbsolute = std::filesystem::absolute(Application::GetOpenDocumentDirectory() / currentScene->GetFilepath());

		bool affected = false;
		for (const std::filesystem::path& updated : updatedFiles)
		{
			std::error_code errorCode;
			if (std::filesystem::equivalent(updated, currentSceneAbsolute, errorCode))
			{
				affected = true;
				break;
			}
		}

		if (!affected)
			return;

		if (currentScene->IsDirty())
		{
			ENGINE_WARN("'{0}' references a renamed/converted asset but has unsaved changes - save and reopen it to pick up the update.", currentSceneAbsolute.filename().string());
			return;
		}

		SceneManager::ChangeScene(currentScene->GetFilepath());
	}
}
