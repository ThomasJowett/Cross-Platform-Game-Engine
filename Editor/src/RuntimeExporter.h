#include "Core/core.h"
#include "Scene/Scene.h"
#include "ProjectData.h"

#include <filesystem>

class RuntimeExporter
{
public:
    void Init(std::filesystem::path exportLocation);
    void ExportGame();
    void ExportScene(Ref<Scene> scene);
private:
    std::filesystem::path m_ExportLocation;
    ProjectData m_Data;
};