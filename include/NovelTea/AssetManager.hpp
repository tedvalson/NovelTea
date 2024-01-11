#ifndef NOVELTEA_ASSETMANAGER_HPP
#define NOVELTEA_ASSETMANAGER_HPP

#include <NovelTea/Err.hpp>
#include <memory>
#include <map>
#include <assert.h>

namespace NovelTea
{

struct AssetPath
{
	static void set(const std::string &dirName);
	static const std::string &get();
private:
	static std::string path;
};

template <class T>
class AssetManager
{
public:
	static std::shared_ptr<T> get(const std::string& filename)
	{
		static AssetManager<T> manager; // Yep, singleton

		auto item = manager.m_assets.find(filename);
		if (item == manager.m_assets.end())
		{
			std::shared_ptr<T> asset(new T);
#ifdef ANDROID
			auto fullPath = filename;
#else
			auto fullPath = AssetPath::get() + filename;
#endif
			if (!asset->loadFromFile(fullPath))
			{
				err() << "Failed to load asset: " << fullPath << std::endl;
				return nullptr;
			}
			manager.m_assets.insert(std::make_pair(filename, asset));
			return get(filename);
		}
		return item->second;
	}

private:
	AssetManager<T>() {} // Empty constructor

	std::map<std::string, std::shared_ptr<T>> m_assets;
};

} // namespace NovelTea

#endif // NOVELTEA_ASSETMANAGER_HPP
