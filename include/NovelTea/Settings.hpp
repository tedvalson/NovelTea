#ifndef NOVELTEA_SETTINGS_HPP
#define NOVELTEA_SETTINGS_HPP

#include <NovelTea/json.hpp>
#include <NovelTea/Profile.hpp>
#include <NovelTea/Utils.hpp>
#include <memory>

#define GSettings NovelTea::Settings::get()

namespace NovelTea
{

class Settings
{
public:
	static Settings &get();
	void load();
	void save() const;

	void setActiveProfileIndex(int index);
	int getActiveProfileIndex() const;
	void addProfile();
	void removeProfile(int index);

	const std::vector<std::unique_ptr<Profile>> &getProfiles() const;

	void setDirectory(const std::string &dirName);
	const std::string &getDirectory() const;

	ADD_ACCESSOR(bool, SaveEnabled, m_saveEnabled)
	ADD_ACCESSOR(float, FontSizeMultiplier, m_fontSizeMultiplier)

protected:
	Settings();

	void reloadProfiles();

private:
	mutable sj::JSON m_json;
	std::string m_directory;
	bool m_saveEnabled;

	std::vector<std::unique_ptr<Profile>> m_profiles;

	float m_fontSizeMultiplier;
	int m_activeProfileIndex;
};

} // namespace NovelTea

#endif // NOVELTEA_SETTINGS_HPP
