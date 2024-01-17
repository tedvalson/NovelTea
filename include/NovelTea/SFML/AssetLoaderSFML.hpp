#ifndef NOVELTEA_ASSETLOADER_SFML_HPP
#define NOVELTEA_ASSETLOADER_SFML_HPP

#include <NovelTea/AssetLoader.hpp>
#include <memory>
#include <map>

#define Asset GSys(AssetLoaderSFML)

namespace sf {
class Font;
class Shader;
class Texture;
}

namespace NovelTea {

class AssetLoaderSFML : public AssetLoader {
public:
	AssetLoaderSFML(Context *context);

	void reset() override;

	std::shared_ptr<sf::Font> font(const std::string &fontName = "sys") const;

	std::shared_ptr<sf::Texture> texture(const std::string &textureName) const;

	std::shared_ptr<sf::Shader> shader(const std::string &fragShaderId, const std::string &vertShaderId = std::string()) const;
	std::shared_ptr<sf::Shader> shader(int systemShaderIndex) const;

private:
	std::map<std::string, std::shared_ptr<sf::Font>> m_fonts;
	std::map<std::string, std::shared_ptr<sf::Texture>> m_textures;
};

} // namespace NovelTea

#endif // NOVELTEA_ASSETLOADER_SFML_HPP
