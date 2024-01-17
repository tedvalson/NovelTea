#include <NovelTea/AssetLoader.hpp>

namespace NovelTea {

std::string AssetLoader::SubsystemName = "AssetLoader";

AssetLoader::AssetLoader(Context *context)
: ContextObject(context)
{
}

AssetLoader::~AssetLoader()
{
}

void AssetLoader::reset()
{
}

}
