#ifndef NOVELTEA_ASSETLOADER_HPP
#define NOVELTEA_ASSETLOADER_HPP

#include <NovelTea/ContextObject.hpp>
#include <string>

namespace NovelTea {

// Loader for project fonts and textures. Does nothing itself.
// Base class for platform-specific implementation.

class AssetLoader : public ContextObject {
public:
	AssetLoader(Context *context);
	virtual ~AssetLoader();

	static std::string SubsystemName;

	virtual void reset();
};

} // namespace NovelTea

#endif // NOVELTEA_ASSETLOADER_HPP
