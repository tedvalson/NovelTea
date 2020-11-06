#include <NovelTea/SaveData.hpp>
#include <NovelTea/ProjectDataIdentifiers.hpp>
#include <NovelTea/Action.hpp>
#include <NovelTea/Cutscene.hpp>
#include <NovelTea/Dialogue.hpp>
#include <NovelTea/Object.hpp>
#include <NovelTea/Room.hpp>
#include <NovelTea/Script.hpp>
#include <NovelTea/Verb.hpp>
#include <SFML/System/FileInputStream.hpp>
#include <fstream>
#include <iostream>

namespace NovelTea
{

SaveData::SaveData()
	: m_directory(".")
{
}

bool SaveData::isLoaded() const
{
	return m_loaded;
}

void SaveData::saveToFile(const std::string &filename)
{
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
		sf::FileInputStream file;
		std::string s;
		if (!file.open(filename))
			return false;

		s.resize(file.getSize());
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
	m_json = sj::Object();
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
	m_directory = path;
}

const std::string &SaveData::getDirectory() const
{
	return m_directory;
}

void SaveData::save(int slot)
{
	saveToFile(getSlotFilename(slot));
}

bool SaveData::load(int slot)
{
	return loadFromFile(getSlotFilename(slot));
}

std::string SaveData::getSlotFilename(int slot) const
{
	return m_directory + "/" + std::to_string(slot) + ".ntsav";
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
