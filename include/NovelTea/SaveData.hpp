#ifndef NOVELTEA_SAVEDATA_HPP
#define NOVELTEA_SAVEDATA_HPP

#include <NovelTea/JsonSerializable.hpp>
#include <NovelTea/ProjectData.hpp>
#include <NovelTea/Entity.hpp>

namespace NovelTea
{

class SaveData: public JsonSerializable
{
	friend class Game;
protected:
	SaveData();
public:
	bool isLoaded() const;

	void saveToFile(const std::string &filename = std::string());
	bool loadFromFile(const std::string &filename);
	const std::string &filename() const;

	std::string getParentId(const std::string &entityType, const std::string &entityId);

	// Reset to conform to loaded project
	void reset();

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

	void set(std::shared_ptr<Entity> obj, const std::string &idName = std::string());

	template <typename T>
	bool exists(const std::string &idName)
	{
		if (idName.empty())
			return false;
		return (ProjData[T::id].hasKey(idName) || data()[T::id].hasKey(idName));
	}

	template <typename T>
	std::shared_ptr<T> get(const std::string &idName)
	{
		auto result = std::make_shared<T>();
		if (exists<T>(idName))
		{
			if (data()[T::id].hasKey(idName))
				result->fromJson(data()[T::id][idName]);
			else
				result->fromJson(ProjData[T::id][idName]);
		}
		return result;
	}

private:
	bool m_loaded = false;
	std::string m_directory;
	std::string m_filename;
	mutable json m_json;
};

} // namespace NovelTea

#endif // NOVELTEA_SAVEDATA_HPP
