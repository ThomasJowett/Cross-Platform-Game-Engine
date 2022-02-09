#include "stdafx.h"
#include "FileUtils.h"

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
