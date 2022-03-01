#include "RuntimeExporter.h"
#include <fstream>

#include "cereal/archives/json.hpp"
#include "cereal/types/string.hpp"
#include "Utilities/FileUtils.h"
#include "Core/Application.h"


#include "FileSystem/Directory.h"

void RuntimeExporter::Init(std::filesystem::path exportLocation)
{
    m_ExportLocation = exportLocation;

    std::ifstream file(Application::GetOpenDocument());

    if (!file.is_open()) return;

    cereal::JSONInputArchive input(file);
    input(m_Data);

    file.close();
}

void RuntimeExporter::ExportGame() 
{
    // Export all the scenes within the project folder
    std::vector<std::string> fileExtensions = {".scene"};
    auto sceneFiles = Directory::GetFilesRecursive(Application::GetOpenDocumentDirectory(), fileExtensions);

    for (std::filesystem::path sceneFile : sceneFiles)
    {
        Ref<Scene> scene = CreateRef<Scene>(sceneFile);
        ExportScene(scene);
    }
}

void RuntimeExporter::ExportScene(Ref<Scene> scene)
{
    std::filesystem::path relativePath = FileUtils::RelativePath(scene->GetFilepath(), Application::GetOpenDocumentDirectory());
    std::filesystem::path finalPath = m_ExportLocation / relativePath;
    scene->Save(finalPath, true);
}
