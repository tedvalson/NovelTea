#include <NovelTea/SaveData.hpp>
#include <NovelTea/ProjectDataIdentifiers.hpp>
#include <NovelTea/Diff.hpp>
#include <NovelTea/Action.hpp>
#include <NovelTea/Cutscene.hpp>
#include <NovelTea/Dialogue.hpp>
#include <NovelTea/Object.hpp>
#include <NovelTea/Room.hpp>
#include <NovelTea/Script.hpp>
#include <NovelTea/Verb.hpp>
#include <NovelTea/FileUtils.hpp>
#include <NovelTea/Settings.hpp>
#include <SFML/System/FileInputStream.hpp>
#include <fstream>
#include <iostream>

namespace {
	const auto lastFilename = "lastSave";
}

namespace NovelTea
{

SaveData::SaveData()
	: m_loaded(false)
	, m_saveEnabled(false)
	, m_directory(".")
	, m_profileIndex(-1)
{
	m_json = sj::JSON({
		ID::playTime, 0.f,
	});
}

bool SaveData::isLoaded() const
{
	return m_loaded;
}

void SaveData::saveToFile(const std::string &filename)
{
	if (!m_saveEnabled)
		return;
	if (filename.empty() && m_filename.empty())
		return;
	std::ofstream file(filename.empty() ? m_filename : filename);
	auto j = toJson();
//	json::to_msgpack(j, file);
	file << j;
}

bool SaveData::loadFromFile(const std::string &filename)
{
	try
	{
		std::ifstream file(filename);
		if (!file.is_open())
			return false;

		std::string s;
		file.seekg(0, std::ios_base::end);
		s.resize(file.tellg());
		file.seekg(0);
		file.read(&s[0], s.size());
		auto j = json::Load(s);

//		auto j = json::from_msgpack(file);
		auto success = fromJson(j);
		if (success)
			m_filename = filename;
		return success;
	}
	catch (std::exception &e)
	{
		std::cout << "Failed to load game save: " << filename << std::endl;
		std::cout << e.what() << std::endl;
		return false;
	}
}

const std::string &SaveData::filename() const
{
	return m_filename;
}

std::string SaveData::getParentId(const std::string &entityType, const std::string &entityId)
{
	if (entityType.empty())
		return std::string();

	json j;
	if (data()[entityType].hasKey(entityId))
		j = data()[entityType][entityId];
	else
		j = ProjData[entityType][entityId];
	return j[1].ToString();
}

void SaveData::reset()
{
	if (!Proj.isLoaded())
		return;
	m_json = sj::JSON({
		ID::playTime, 0.f,
		ID::navigationEnabled, true,
	});
	m_json[ID::objectLocations][Room::id] = Room::getProjectRoomObjects();
	m_json[ID::roomDescriptions] = sj::Object();
	m_json[ID::properties] = sj::Object();
	resetRoomDescriptions();
}

std::string SaveData::roomDescription(const std::string &id, const std::string &newDescription)
{
	auto &j = m_json[ID::roomDescriptions];
	auto prevDescription = j.hasKey(id) ? j[id].ToString(): "";
	j[id] = newDescription;
	return diff(prevDescription, newDescription);
}

void SaveData::resetRoomDescriptions()
{
	m_json[ID::roomDescriptions] = sj::Object();
}

json SaveData::toJson() const
{
//	_json[NT_ENGINE_VERSION] = m_engineVersion;
	return m_json;
}

bool SaveData::fromJson(const json &j)
{
	m_loaded = false;
	m_filename.clear();

	m_json = j;
	m_loaded = true;
	return true;
}

const json &SaveData::data() const
{
	return m_json;
}

json &SaveData::data()
{
	return m_json;
}

void SaveData::setDirectory(const std::string &path)
{
	m_saveEnabled = !path.empty();
	m_directory = path;
}

const std::string &SaveData::getDirectory() const
{
	return m_directory;
}

void SaveData::save(int slot)
{
	if (!m_saveEnabled)
		return;
	if (m_profileIndex < 0)
		GSettings.ensureProfileExists();
	saveToFile(getSlotFilename(slot));

	std::ofstream file(getProfileDirName() + "/" + lastFilename);
	file << slot;
}

bool SaveData::load(int slot)
{
	if (!m_saveEnabled)
		return true;
	return loadFromFile(getSlotFilename(slot));
}

bool SaveData::loadLast()
{
	if (!m_saveEnabled)
		return true;
	int slot;
	std::ifstream file(getProfileDirName() + "/" + lastFilename);
	if (file.is_open()) {
		file >> slot;
		return load(slot);
	}

	return false;
}

std::string SaveData::getSlotFilename(int slot) const
{
	return getProfileDirName() + "/" + std::to_string(slot) + ".ntsav";
}

std::string SaveData::getProfileDirName() const
{
	return getProfileDirName(m_profileIndex);
}

std::string SaveData::getProfileDirName(int index) const
{
	return m_directory + "/" + std::to_string(index);
}

void SaveData::setProfileIndex(int index)
{
	if (index < 0)
		return;
	m_profileIndex = index;
	if (m_saveEnabled)
		createDir(getProfileDirName());
	reset();
}

int SaveData::getProfileIndex() const
{
	return m_profileIndex;
}

void SaveData::removeProfile(int index, int profileCount)
{
	removeDir(getProfileDirName(index));

	// Move save files
	for (int i = index+1; i < profileCount; ++i)
	{
		if (!dirExists(getProfileDirName(i)))
			continue;
		if (!moveDir(getProfileDirName(i), getProfileDirName(i-1)))
		{
			std::cout << "ERROR: Failed to move dir (" << getProfileDirName(i) << ")" << std::endl;
			return;
		}
	}
}

void SaveData::set(std::shared_ptr<Entity> obj, const std::string &idName)
{
	if (!idName.empty())
		obj->setId(idName);
	else if (obj->getId().empty())
		return;
	data()[obj->entityId()][obj->getId()] = obj->toJson();
}

} // namespace NovelTea
