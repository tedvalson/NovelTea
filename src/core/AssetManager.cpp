#include <NovelTea/AssetManager.hpp>

std::string NovelTea::AssetPath::path;

void NovelTea::AssetPath::set(const std::string &dirName)
{
	path = dirName;
}

const std::string &NovelTea::AssetPath::get()
{
	return path;
}
