#include "FileUtils.h"

#include <algorithm>
#include <cctype>
#include <string>
#include <vector>

std::filesystem::path FileUtils::RelativePath(const std::filesystem::path& path, const std::filesystem::path& relative_to)
{
    // create absolute paths
    std::filesystem::path p = std::filesystem::absolute(path);
    std::filesystem::path r = std::filesystem::absolute(relative_to);

    // if root paths are different, return absolute path
    if (p.root_path() != r.root_path())
        return p;

    // initialize relative path
    std::filesystem::path result;

    // find out where the two paths diverge
    std::filesystem::path::const_iterator itr_path = p.begin();
    std::filesystem::path::const_iterator itr_relative_to = r.begin();
    while (itr_path != p.end() && itr_relative_to != r.end() && *itr_path == *itr_relative_to) {
        ++itr_path;
        ++itr_relative_to;
    }

    // add "../" for each remaining token in relative_to
    if (itr_relative_to != r.end()) {
        ++itr_relative_to;
        while (itr_relative_to != r.end()) {
            result /= "..";
            ++itr_relative_to;
        }
    }

    // add remaining path
    while (itr_path != p.end()) {
        result /= *itr_path;
        ++itr_path;
    }

    return result;
}

/* ------------------------------------------------------------------------------------------------------------------ */

bool FileUtils::IsAbsolutePath(const std::filesystem::path& path)
{
	if (path.is_absolute() || path.has_root_directory())
		return true;

	const std::string pathString = path.generic_string();
	return pathString.size() >= 3 && std::isalpha((unsigned char)pathString[0]) && pathString[1] == ':'
		&& (pathString[2] == '/' || pathString[2] == '\\');
}

/* ------------------------------------------------------------------------------------------------------------------ */

std::filesystem::path FileUtils::ToProjectRelative(const std::filesystem::path& path, const std::filesystem::path& projectDirectory)
{
	if (projectDirectory.empty() || !IsAbsolutePath(path))
		return path;

	std::filesystem::path projectRoot = std::filesystem::absolute(projectDirectory).lexically_normal();
	if (path.is_absolute())
	{
		std::filesystem::path relative = path.lexically_normal().lexically_relative(projectRoot);
		if (!relative.empty() && *relative.begin() != "..")
			return relative;
	}

	if (std::filesystem::exists(path))
		return path;

	std::string pathString = path.generic_string();
	std::replace(pathString.begin(), pathString.end(), '\\', '/');

	std::vector<std::string> parts;
	size_t start = 0;
	while (start <= pathString.size())
	{
		size_t end = pathString.find('/', start);
		if (end == std::string::npos)
			end = pathString.size();
		std::string part = pathString.substr(start, end - start);
		if (!part.empty() && !(part.size() == 2 && part[1] == ':'))
			parts.push_back(part);
		start = end + 1;
	}

	for (size_t first = 0; first < parts.size(); ++first)
	{
		std::filesystem::path tail;
		for (size_t i = first; i < parts.size(); ++i)
			tail /= parts[i];
		if (std::filesystem::exists(projectRoot / tail))
			return tail;
	}

	return path;
}
