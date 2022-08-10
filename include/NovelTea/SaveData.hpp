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
	friend class Settings;
protected:
	SaveData();

	void save(int slot);
	bool load(int slot);
	bool loadLast();
	std::string getSlotFilename(int slot) const;
	std::string getProfileDirName() const;
	std::string getProfileDirName(int index) const;

	void setProfileIndex(int index);
	int getProfileIndex() const;
	void removeProfile(int index, int profileCount);

public:
	bool isLoaded() const;

	void saveToFile(const std::string &filename = std::string());
	bool loadFromFile(const std::string &filename);
	const std::string &filename() const;

	// Reset to conform to loaded project
	void reset();

	std::string roomDescription(const std::string &id, const std::string &newDescription);
	void resetRoomDescriptions();

	json toJson() const override;
	bool fromJson(const json &j) override;

	const json &data() const;
	json &data();

	void setDirectory(const std::string &path);
	const std::string &getDirectory() const;

	void set(std::shared_ptr<Entity> obj, const std::string &idName = std::string());

private:
	bool m_loaded;
	bool m_saveEnabled;
	std::string m_directory;
	std::string m_filename;
	mutable json m_json;
	int m_profileIndex;
};

} // namespace NovelTea

#endif // NOVELTEA_SAVEDATA_HPP
