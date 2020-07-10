#ifndef NOVELTEA_SAVEDATA_HPP
#define NOVELTEA_SAVEDATA_HPP

#include <NovelTea/JsonSerializable.hpp>
#include <NovelTea/ProjectData.hpp>

#define Save NovelTea::SaveData::instance()

namespace NovelTea
{

class SaveData: public JsonSerializable
{
public:
	static SaveData &instance();

	bool isLoaded() const;

	void saveToFile(const std::string &filename = std::string());
	bool loadFromFile(const std::string &filename);
	const std::string &filename() const;

	json toJson() const override;
	bool fromJson(const json &j) override;

	const json &data() const;
	json &data();

	void writeVariables(const std::string &jsonData);
	std::string readVariables(const std::string &jsonData);

	void setDirectory(const std::string &path);
	const std::string &getDirectory() const;

	void save(int slot);
	bool load(int slot);
	std::string getSlotFilename(int slot) const;

	template <typename T>
	static bool exists(const std::string &idName)
	{
		if (idName.empty())
			return false;
		return (ProjData[T::id].contains(idName) || Save.data()[T::id].contains(idName));
	}

	template <typename T>
	static std::shared_ptr<T> get(const std::string &idName)
	{
		if (!exists<T>(idName))
			return std::make_shared<T>();
		if (Save.data()[T::id].contains(idName))
			return std::make_shared<T>(Save.data()[T::id][idName].template get<T>());
		else
			return std::make_shared<T>(ProjData[T::id][idName].template get<T>());
	}

	template <typename T>
	static void set(std::shared_ptr<T> obj, const std::string &idName = std::string())
	{
		if (!idName.empty())
			obj->setId(idName);
		else if (obj->getId().empty())
			return;
		Save.data()[T::id][obj->getId()] = *obj;
	}

protected:
	SaveData();

private:
	bool m_loaded = false;
	std::string m_directory;
	std::string m_filename;
	mutable json m_json;
};

} // namespace NovelTea

#endif // NOVELTEA_SAVEDATA_HPP
