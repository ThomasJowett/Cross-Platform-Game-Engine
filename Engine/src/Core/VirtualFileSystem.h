#pragma once
#include <miniz.h>

class VirtualFileSystem {
public:
	void Mount(const void* zipData, size_t zipSize);
	void Unmount();

	bool Exists(const std::string& path);
	bool ReadFile(const std::string& path, std::vector<uint8_t>& data);

private:
	mz_zip_archive m_Archive;
	std::unordered_map<std::string, mz_uint> m_FileIndex;
};
