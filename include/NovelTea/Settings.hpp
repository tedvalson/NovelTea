#ifndef NOVELTEA_SETTINGS_HPP
#define NOVELTEA_SETTINGS_HPP

#include <NovelTea/ContextObject.hpp>
#include <NovelTea/json.hpp>
#include <NovelTea/Profile.hpp>
#include <NovelTea/Utils.hpp>
#include <memory>

namespace NovelTea
{

class Settings : public ContextObject
{
public:
	Settings(Context* context);
	void load();
	void save() const;

	void setActiveProfileIndex(int index);
	int getActiveProfileIndex();
	std::shared_ptr<Profile> getActiveProfile();
	void addProfile(const std::string &text);
	void removeProfile(int index);
	void ensureProfileExists();

	const std::vector<std::shared_ptr<Profile>> &getProfiles() const;

	void setDirectory(const std::string &dirName);
	const std::string &getDirectory() const;

	ADD_ACCESSOR(bool, SaveEnabled, m_saveEnabled)
	ADD_ACCESSOR(float, FontSizeMultiplier, m_fontSizeMultiplier)

protected:
	void reloadProfiles();

private:
	mutable sj::JSON m_json;
	std::string m_directory;
	bool m_saveEnabled;

	std::vector<std::shared_ptr<Profile>> m_profiles;

	float m_fontSizeMultiplier;
	int m_activeProfileIndex;
};

} // namespace NovelTea

#endif // NOVELTEA_SETTINGS_HPP
