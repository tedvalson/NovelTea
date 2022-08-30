#ifndef NOVELTEA_PROJECTDATA_HPP
#define NOVELTEA_PROJECTDATA_HPP

#include <NovelTea/JsonSerializable.hpp>
#include <NovelTea/Utils.hpp>
#include <SFML/System/String.hpp>
#include <memory>

namespace sf
{
class Font;
class Shader;
class Texture;
}

namespace NovelTea
{

class Action;
class Context;
class Entity;

class ProjectData: public JsonSerializable
{
public:
	ProjectData();
	ProjectData(const std::string &fileName);
	virtual ~ProjectData();

	void newProject();
	void closeProject();
	void clearFilename();

	bool isLoaded() const;
	bool isValid(std::string &errorMessage) const;

	void renameEntity(Context *context, EntityType entityType, const std::string &oldName, const std::string &newName);

	void setFontData(const std::string &alias, const std::string &data);
	const std::string &getFontData(const std::string &alias) const;
	std::shared_ptr<sf::Font> getFont(const std::string &fontName = "sys") const;

	void setTextureData(const std::string &name, const std::string &data);
	const std::string &getTextureData(const std::string &name) const;
	std::shared_ptr<sf::Texture> getTexture(const std::string &name) const;

	std::shared_ptr<sf::Shader> getShader(const std::string &fragShaderId, const std::string &vertShaderId = std::string());
	std::shared_ptr<sf::Shader> getShader(int systemShaderIndex);

	void saveToFile(const std::string &fileName = std::string());
	bool loadFromFile(const std::string &fileName);
	const std::string &fileName() const;

	void setImageData(const std::string &data);
	const std::string &getImageData() const;

	json toJson() const override;
	bool fromJson(const json &j) override;

	const json &data() const;
	json &data();

	void set(std::shared_ptr<Entity> obj, const std::string &idName = std::string());

	template <typename T>
	std::shared_ptr<T> get(const std::string &idName, Context *context)
	{
		if (idName.empty())
			return nullptr;
		if (!m_json[T::id].hasKey(idName))
			return nullptr;
		auto result = std::make_shared<T>(context);
		result->fromJson(m_json[T::id][idName]);
		return result;
	}

private:
	bool m_loaded;
	std::string m_fileName;
	mutable json m_json;

	std::map<std::string, std::shared_ptr<sf::Font>> m_fonts;
	std::map<std::string, std::shared_ptr<sf::Texture>> m_textures;
	std::map<std::string, std::string> m_fontsData;
	std::map<std::string, std::string> m_texturesData;
	std::string m_imageData;
};

} // namespace NovelTea

#endif // NOVELTEA_PROJECTDATA_HPP
