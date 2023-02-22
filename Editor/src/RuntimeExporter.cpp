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

	m_ExportLocation.remove_filename();

	m_GameName = exportLocation.filename();

#ifdef __WINDOWS__
	m_GameName.replace_extension(".exe");
#endif // __WINDOWS__

	std::ifstream file(Application::GetOpenDocument());

	if (!file.is_open()) return;

	cereal::JSONInputArchive input(file);
	input(m_Data);

	file.close();
}

void RuntimeExporter::ExportGame()
{
	// --------------------------------------------
	// TODO: properly do this part
	try
	{
		std::filesystem::copy(Application::GetOpenDocumentDirectory(), m_ExportLocation,
			std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing);
	}
	catch (const std::exception& e)
	{
		CLIENT_ERROR(e.what());
	}
	// --------------------------------------------

	std::filesystem::remove(m_ExportLocation / Application::GetOpenDocument().filename());

	// Copy the runtime executable 
	try
	{
		if (std::filesystem::exists(Application::GetWorkingDirectory() / "runtime" / "Runtime.exe"))
		{
			std::filesystem::copy_file(Application::GetWorkingDirectory() / "runtime" / "Runtime.exe",
				m_ExportLocation / m_GameName,
				std::filesystem::copy_options::overwrite_existing);
		}
		else if (std::filesystem::exists(Application::GetWorkingDirectory() / "runtime" / "Runtime"))
		{
			std::filesystem::copy_file(Application::GetWorkingDirectory() / "runtime" / "Runtime",
				m_ExportLocation / m_GameName,
				std::filesystem::copy_options::overwrite_existing);
		}
	}
	catch (std::exception& e)
	{
		CLIENT_ERROR(e.what());
	}

	// Copy the shaders 
	// TODO: create distributable binaries using SPIR-V Cross
	try
	{
		std::filesystem::copy(Application::GetWorkingDirectory() / "data",
			m_ExportLocation / "data",
			std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing);
	}
	catch (std::exception& e)
	{
		CLIENT_ERROR(e.what());
	}

	// Create the startup file
	std::filesystem::path startupFilename = m_ExportLocation / "Startup";
	std::ofstream outbin(startupFilename, std::ios::out | std::ios::binary);
	std::string gameName = m_GameName.string();
	gameName = gameName.substr(0, gameName.find_last_of('.'));
	size_t gameNameSize = gameName.size();
	outbin.write((char*)&gameNameSize, sizeof(gameNameSize));
	outbin.write((char*)&gameName[0], gameNameSize);

	size_t sceneNameSize = m_Data.defaultScene.size();
	outbin.write((char*)&sceneNameSize, sizeof(sceneNameSize));
	outbin.write((char*)&m_Data.defaultScene[0], sceneNameSize);
	outbin.close();

	

	
	// TODO: form a list of all lua scripts and copy them
	// TODO: form a list of all textures and copy them
	// TODO: form a list of all static meshes and copy them
}
