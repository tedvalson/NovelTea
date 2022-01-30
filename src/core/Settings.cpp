#include <NovelTea/Settings.hpp>
#include <NovelTea/FileUtils.hpp>
#include <NovelTea/Game.hpp>
#include <NovelTea/SaveData.hpp>
#include <SFML/System/Err.hpp>
#include <fstream>

namespace
{
	const auto fileName = "settings.conf";
	const auto propFontSizeMultiplier = "sizeFactor";
	const auto propActiveProfile = "activeProfile";
	const auto propProfiles = "profiles";
}

namespace NovelTea
{

Settings::Settings()
: m_directory("./")
, m_saveEnabled(false)
, m_fontSizeMultiplier(1.f)
, m_activeProfileIndex(-1)
{
	reloadProfiles();
}

void Settings::reloadProfiles()
{
	m_profiles.clear();
	int index = 1;
	for (auto &jprofile : m_json[propProfiles].ArrayRange())
	{
		auto profile = new Profile(index++);
		m_profiles.emplace_back(profile);
	}

	if (m_profiles.empty())
	{
		addProfile();
		setActiveProfileIndex(0);
	}
}

Settings &Settings::get()
{
	static Settings obj;
	return obj;
}

void Settings::load()
{
	try
	{
		std::ifstream file(m_directory + fileName);
		if (!file.is_open())
			return;

		std::string s;
		file.seekg(0, std::ios_base::end);
		s.resize(file.tellg());
		file.seekg(0);
		file.read(&s[0], s.size());

		m_json = sj::JSON::Load(s);
		reloadProfiles();
		m_fontSizeMultiplier = m_json[propFontSizeMultiplier].ToFloat();
		setActiveProfileIndex(m_json[propActiveProfile].ToInt());
	}
	catch (std::exception &e)
	{
		sf::err() << "Failed to load game settings." << std::endl;
		sf::err() << e.what() << std::endl;
	}
}

void Settings::save() const
{
	auto jprofiles = sj::Array();
	for (auto &profile : m_profiles)
		jprofiles.append(sj::Array());
	m_json = sj::JSON({
		propFontSizeMultiplier, m_fontSizeMultiplier,
		propProfiles, jprofiles,
		propActiveProfile, m_activeProfileIndex,
	});

	if (!m_saveEnabled)
		return;
	std::ofstream file(m_directory + fileName);
	if (!file.is_open())
		return;
	file << m_json;
}

void Settings::setActiveProfileIndex(int index)
{
	if (index >= m_profiles.size() || index == m_activeProfileIndex)
		return;
	m_activeProfileIndex = index;
	GSave->setProfileIndex(index);
	save();
}

int Settings::getActiveProfileIndex() const
{
	return m_activeProfileIndex;
}

void Settings::addProfile()
{
	auto profile = new Profile(0);
	m_profiles.emplace_back(profile);
	save();
}

void Settings::removeProfile(int index)
{
	GSave->removeProfile(index, m_profiles.size());
	m_profiles.erase(m_profiles.begin() + index);
	if (m_profiles.empty())
	{
		addProfile();
		setActiveProfileIndex(0);
	}
	else if (m_activeProfileIndex == index && index >= m_profiles.size())
	{
		setActiveProfileIndex(m_profiles.size() - 1);
	}
}

const std::vector<std::unique_ptr<Profile> > &Settings::getProfiles() const
{
	return m_profiles;
}

void Settings::setDirectory(const std::string &dirName)
{
	m_saveEnabled = !dirName.empty();
	if (!dirExists(dirName)) {
		sf::err() << "Directory does not exist for Settings: '" << dirName << "'" << std::endl;
		return;
	}
	m_directory = dirName;
	if (!endsWith(dirName, "/"))
		m_directory += "/";
}

const std::string &Settings::getDirectory() const
{
	return m_directory;
}

} // namespace NovelTea
