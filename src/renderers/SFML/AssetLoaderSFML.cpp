#include <NovelTea/SFML/AssetLoaderSFML.hpp>
#include <NovelTea/AssetManager.hpp>
#include <NovelTea/Context.hpp>
#include <NovelTea/Game.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace NovelTea
{

AssetLoaderSFML::AssetLoaderSFML(Context *context)
: AssetLoader(context)
{
}

void AssetLoaderSFML::reset()
{
	m_textures.clear();
	m_fonts.clear();

	for (auto &jfont : ProjData[ID::engineFonts].ObjectRange())
	{
		auto font = AssetManager<sf::Font>::get("fonts/" + jfont.second.ToString());
		if (font)
			m_fonts[jfont.first] = font;
	}

	// Load fonts
	for (auto& jfont : ProjData[ID::projectFonts].ObjectRange()) {
		auto& alias = jfont.first;
		auto font = std::make_shared<sf::Font>();
		auto &data = Proj->getFontData(alias);
		if (font->loadFromMemory(data.data(), data.size()))
			m_fonts[alias] = font;
		else {
			err() << "Failed to load project font: " << jfont.second.ToString() << std::endl;
			auto &defaultFont = ProjData[ID::projectFontDefault];
			if (alias == defaultFont.ToString())
				defaultFont = "sys";
		}
	}
	// Load textures
	for (auto& jtexture : ProjData[ID::textures].ObjectRange()) {
		auto& name = jtexture.first;
		auto texture = std::make_shared<sf::Texture>();
		auto &data = Proj->getTextureData(name);
		texture->flip(true);
		texture->setRepeated(true);
		if (texture->loadFromMemory(data.data(), data.size())) {
			m_textures[name] = texture;
		} else {
			err() << "Failed to load project texture: " << jtexture.second.ToString() << std::endl;
		}
	}
}

std::shared_ptr<sf::Font> AssetLoaderSFML::font(const std::string &fontName) const
{
	if (m_fonts.find(fontName) == m_fonts.end())
		return m_fonts.at("sys");
	return m_fonts.at(fontName);
}

std::shared_ptr<sf::Texture> AssetLoaderSFML::texture(const std::string &textureName) const
{
	if (m_textures.find(textureName) == m_textures.end())
		return nullptr;
	return m_textures.at(textureName);
}

std::shared_ptr<sf::Shader> AssetLoaderSFML::shader(const std::string &fragShaderId, const std::string &vertShaderId) const
{
	auto& shaders = ProjData[ID::shaders];
	auto shader = std::make_shared<sf::Shader>();
	auto fragShader = shaders[shaders.hasKey(fragShaderId) ? fragShaderId : "defaultFrag"];
	auto vertShader = shaders[shaders.hasKey(vertShaderId) ? vertShaderId : "defaultVert"];
	if (shader->loadFromMemory(vertShader[0].ToString(), fragShader[0].ToString()))
	{
		auto loadUniforms = [this, &shader](json &jshader) {
			for (auto& j : jshader.ObjectRange()) {
				// Uniforms is either a float or a string (with texture id)
				bool ok;
				float value = j.second.ToFloat(ok);
				if (ok)
					shader->setUniform(j.first, value);
				else {
					auto tex = texture(j.second.ToString());
					if (tex) {
						shader->setUniform(j.first, *tex);
						tex->setRepeated(true);
						shader->setUniform(j.first + "Matrix", tex->getMatrix(sf::Texture::Normalized));
					}
				}
			}
		};

		shader->setUniform("texture", sf::Shader::CurrentTexture);
		loadUniforms(fragShader[1]);
		loadUniforms(vertShader[1]);
	}
	return shader;
}

std::shared_ptr<sf::Shader> AssetLoaderSFML::shader(int systemShaderIndex) const
{
	return shader(ProjData[ID::systemShaders][systemShaderIndex].ToString());
}

} // namespace NovelTea
