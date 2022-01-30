#ifndef NOVELTEA_FILEUTILS_HPP
#define NOVELTEA_FILEUTILS_HPP

#include <vector>
#include <string>

namespace NovelTea
{

std::vector<std::string> getFileList(const std::string &path, const std::string &extFilter = std::string());
bool dirExists(const std::string &path);
bool fileExists(const std::string &path);
bool createDir(const std::string &path, bool recreateExisting = false);
bool removeDir(const std::string &path);
bool moveDir(const std::string &oldPath, const std::string &newPath);

bool endsWith(const std::string &s, const std::string &ending);

} // namespace NovelTea

#endif // NOVELTEA_FILEUTILS_HPP
