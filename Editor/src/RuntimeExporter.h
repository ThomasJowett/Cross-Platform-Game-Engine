#include "ProjectData.h"

#include <filesystem>

class RuntimeExporter
{
public:
    void Init(std::filesystem::path exportLocation);
    void ExportGame();
private:
    std::filesystem::path m_ExportLocation;
    std::filesystem::path m_GameName;
    ProjectData m_Data;
};