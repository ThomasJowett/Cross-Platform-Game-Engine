#include "VirtualFileSystem.h"
#include "Logging/Logger.h"
#include "Logging/Instrumentor.h"

VirtualFileSystem::VirtualFileSystem()
{
	PROFILE_FUNCTION();
	mz_zip_zero_struct(&m_Archive);
}

VirtualFileSystem::~VirtualFileSystem()
{
	mz_zip_reader_end(&m_Archive);
}

void VirtualFileSystem::Mount(const void* zipData, size_t zipSize)
{
	PROFILE_FUNCTION();
	if (!mz_zip_reader_init_mem(&m_Archive, zipData, zipSize, 0))
	{
		ENGINE_ERROR("Failed to mount zip archive");
		return;
	}

	mz_uint fileCount = mz_zip_reader_get_num_files(&m_Archive);

	for (mz_uint i = 0; i < fileCount; ++i)
	{
		mz_zip_archive_file_stat fileStat;
		if (mz_zip_reader_file_stat(&m_Archive, i, &fileStat))
		{
			std::string path = fileStat.m_filename;
			m_FileIndex[path] = i;
		}
	}

	ENGINE_INFO("Mounted zip archive with {0} files", fileCount);
}

void VirtualFileSystem::Unmount()
{
	if (mz_zip_reader_end(&m_Archive) == MZ_FALSE)
	{
		ENGINE_ERROR("Failed to unmount zip archive");
	}
	else
	{
		ENGINE_INFO("Unmounted zip archive");
	}
	m_FileIndex.clear();
}

bool VirtualFileSystem::Exists(const std::filesystem::path& path)
{
	return (m_FileIndex.find(path) != m_FileIndex.end());
}

bool VirtualFileSystem::ReadFile(const std::filesystem::path& path, std::vector<uint8_t>& outData)
{
	PROFILE_FUNCTION();
	auto it = m_FileIndex.find(path);
	if (it == m_FileIndex.end())
	{
		ENGINE_ERROR("File not found in VFS: {0}", path);
		return false;
	}

	size_t size = 0;
	void* data = mz_zip_reader_extract_to_heap(&m_Archive, it->second, &size, 0);

	if (!data)
	{
		ENGINE_ERROR("Failed to extract file from VFS: {0}", path);
		return false;
	}

	outData.resize(size);
	std::memcpy(outData.data(), data, size);
	mz_free(data);

	return true;
}
