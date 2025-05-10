#pragma once
#include <miniz.h>
#include <filesystem>

class VirtualFileSystem {
public:
	VirtualFileSystem();
	~VirtualFileSystem();
	void Mount(const void* zipData, size_t zipSize);
	void Unmount();

	bool Exists(const std::filesystem::path& path);
	bool ReadFile(const std::filesystem::path& path, std::vector<uint8_t>& data);

private:
	mz_zip_archive m_Archive;
	std::unordered_map<std::filesystem::path, mz_uint> m_FileIndex;
};
