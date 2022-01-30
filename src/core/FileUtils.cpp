#include <NovelTea/FileUtils.hpp>
#include <sys/stat.h>
#include <dirent.h>

namespace NovelTea
{

std::vector<std::string> getFileList(const std::string &path, const std::string &extFilter)
{
	std::vector<std::string> v;
	if (!dirExists(path))
		return v;

	struct dirent* de;
	auto dirPath = path;
	if (!endsWith(dirPath, "/"))
		dirPath += "/";

	auto dir = opendir(dirPath.c_str());
	if (dir)
	{
		while (de = readdir(dir))
		{
			std::string fileName(de->d_name);
			if (fileName == "." || fileName == "..")
				continue;
			std::string pathAndName = dirPath + fileName;
			if (fileExists(pathAndName))
			{
				if (extFilter.empty() || endsWith(pathAndName, extFilter))
					v.push_back(pathAndName);
			}
		}
		closedir(dir);
	}

	return v;
}

bool dirExists(const std::string &path)
{
	struct stat st;
	if (stat(path.c_str(), &st) || !(st.st_mode & S_IFDIR))
		return false;
	return true;
}

bool fileExists(const std::string &path)
{
	struct stat st;
	if (stat(path.c_str(), &st) || st.st_mode & S_IFDIR)
		return false;
	return true;
}

bool removeDir(const std::string &path)
{
	if (!dirExists(path))
		return true;
	auto files = getFileList(path);
	for (auto &file : files)
		remove(file.c_str());
	return remove(path.c_str()) == 0;
}

bool endsWith(const std::string &s, const std::string &ending)
{
	return s.find(ending) == s.size() - ending.size();
}

bool createDir(const std::string &path, bool recreateExisting)
{
	if (dirExists(path))
	{
		if (recreateExisting)
			removeDir(path);
		else
			return true;
	}
	return mkdir(path.c_str(), S_IRWXU) == 0;
}

bool moveDir(const std::string &oldPath, const std::string &newPath)
{
	return rename(oldPath.c_str(), newPath.c_str()) == 0;
}

} // namespace NovelTea
