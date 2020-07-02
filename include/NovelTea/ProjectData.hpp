#ifndef NOVELTEA_PROJECTDATA_HPP
#define NOVELTEA_PROJECTDATA_HPP

#include <NovelTea/ProjectDataIdentifiers.hpp>
#include <NovelTea/JsonSerializable.hpp>
#include <NovelTea/TextFormat.hpp>
#include <NovelTea/Utils.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/System/String.hpp>

#define Proj NovelTea::ProjectData::instance()
#define ProjData NovelTea::ProjectData::instance().data()

namespace NovelTea
{

class Action;

class ProjectData: public JsonSerializable
{
public:
	static ProjectData &instance();

	void newProject();
	void closeProject();

	bool isLoaded() const;
	bool isValid(std::string &errorMessage) const;

	TextFormat textFormat(size_t index) const;
	size_t addTextFormat(const TextFormat &textFormat);
	bool removeTextFormat(size_t index);

	std::shared_ptr<sf::Font> getFont(size_t index) const;

	template <typename T>
	std::shared_ptr<T> get(const std::string &idName)
	{
		if (idName.empty())
			return nullptr;
		if (!_json[T::id].contains(idName))
			return nullptr;
		return std::make_shared<T>(_json[T::id][idName].get<T>());
	}

	std::shared_ptr<Action> findAction(const std::string &verbId, const std::vector<std::string> &objectIds);

	void saveToFile(const std::string &filename = std::string());
	bool loadFromFile(const std::string &filename);
	const std::string &filename() const;

	json toJson() const override;
	bool fromJson(const json &j) override;

	const json &data() const;
	json &data();

	ADD_ACCESSOR(float, EngineVersion, m_engineVersion)
	ADD_ACCESSOR(sf::String, Title, m_title)

protected:
	ProjectData();
	~ProjectData();

private:
	bool _loaded = false;
	std::string _filename;
	mutable json _json;

	//
	sf::String m_title;
	float m_engineVersion = NOVELTEA_VERSION;
	int m_startTypeId;
	int m_startEntityId;

	std::vector<TextFormat> _textFormats;
	std::vector<std::shared_ptr<sf::Font>> m_fonts;
};

} // namespace NovelTea

#endif // NOVELTEA_PROJECTDATA_HPP
