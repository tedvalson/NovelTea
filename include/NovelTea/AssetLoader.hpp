#ifndef NOVELTEA_ASSETLOADER_HPP
#define NOVELTEA_ASSETLOADER_HPP

#include <NovelTea/Subsystem.hpp>
#include <string>

namespace NovelTea {

// Loader for project fonts and textures. Does nothing itself.
// Base class for platform-specific implementation.

class AssetLoader : public Subsystem {
public:
	AssetLoader(Context *context) : Subsystem(context) {}
	virtual ~AssetLoader() {}

	static std::string name() { return "AssetLoader"; }

	virtual void reset() {}
};

} // namespace NovelTea

#endif // NOVELTEA_ASSETLOADER_HPP
