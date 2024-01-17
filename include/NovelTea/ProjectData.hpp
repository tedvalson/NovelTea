#ifndef NOVELTEA_PROJECTDATA_HPP
#define NOVELTEA_PROJECTDATA_HPP

#include <NovelTea/JsonSerializable.hpp>
#include <NovelTea/Utils.hpp>
#include <memory>

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

	void setTextureData(const std::string &name, const std::string &data);
	const std::string &getTextureData(const std::string &name) const;

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

	std::map<std::string, std::string> m_fontsData;
	std::map<std::string, std::string> m_texturesData;
	std::string m_imageData;
};

} // namespace NovelTea

#endif // NOVELTEA_PROJECTDATA_HPP
