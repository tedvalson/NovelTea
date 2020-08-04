#ifndef NOVELTEA_ASSETMANAGER_HPP
#define NOVELTEA_ASSETMANAGER_HPP

#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/System/Err.hpp>
#include <memory>
#include <map>
#include <assert.h>

namespace NovelTea
{

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
			if (!asset->loadFromFile(filename))
#else
			if (!asset->loadFromFile("/home/android/dev/NovelTea/res/assets/" + filename))
#endif
			{
				sf::err() << "Failed to load asset: " << filename << std::endl;
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
