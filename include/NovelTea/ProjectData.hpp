#ifndef NOVELTEA_PROJECTDATA_HPP
#define NOVELTEA_PROJECTDATA_HPP

#include <NovelTea/JsonSerializable.hpp>
#include <NovelTea/TextFormat.hpp>
#include <NovelTea/Utils.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/System/String.hpp>
#include <memory>

#define Proj NovelTea::ProjectData::instance()
#define ProjData NovelTea::ProjectData::instance().data()

namespace NovelTea
{

class Action;

class ProjectData: public JsonSerializable
{
public:
	ProjectData();
	~ProjectData();
	static ProjectData &instance();

	void newProject();
	void closeProject();
	void clearFilename();

	bool isLoaded() const;
	bool isValid(std::string &errorMessage) const;

	TextFormat textFormat(size_t index) const;
	size_t addTextFormat(const TextFormat &textFormat);
	bool removeTextFormat(size_t index);

	void renameEntity(EntityType entityType, const std::string &oldName, const std::string &newName);

	std::shared_ptr<sf::Font> getFont(size_t index) const;

	template <typename T>
	static std::shared_ptr<T> get(const std::string &idName)
	{
		if (idName.empty())
			return nullptr;
		if (!ProjData[T::id].hasKey(idName))
			return nullptr;
		auto result = std::make_shared<T>();
		result->fromJson(ProjData[T::id][idName]);
		return result;
	}

	template <typename T>
	static void set(std::shared_ptr<T> obj, const std::string &idName = std::string())
	{
		if (!idName.empty())
			obj->setId(idName);
		else if (obj->getId().empty())
			return;
		ProjData[T::id][obj->getId()] = obj->toJson();
	}

	void saveToFile(const std::string &filename = std::string());
	bool loadFromFile(const std::string &filename);
	const std::string &filename() const;

	json toJson() const override;
	bool fromJson(const json &j) override;

	const json &data() const;
	json &data();

private:
	bool _loaded = false;
	std::string _filename;
	mutable json _json;

	std::vector<TextFormat> _textFormats;
	std::vector<std::shared_ptr<sf::Font>> m_fonts;
};

} // namespace NovelTea

#endif // NOVELTEA_PROJECTDATA_HPP
