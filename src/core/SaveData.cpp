#include <NovelTea/SaveData.hpp>
#include <NovelTea/ProjectDataIdentifiers.hpp>
#include <fstream>
#include <iostream>

namespace NovelTea
{

SaveData::SaveData()
{
}

SaveData &SaveData::instance()
{
	static SaveData obj;
	return obj;
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
		std::ifstream file(filename);
		if (!file.is_open())
			return false;
//		auto j = json::from_msgpack(file);
		auto j = json::parse(file);
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

void SaveData::writeVariables(const std::string &jsonData)
{
	auto j = json::parse(jsonData);
	for (auto &item : j.items())
	{
		m_json[ID::variables][item.key()] = item.value();
	}
}

std::string SaveData::readVariables(const std::string &jsonData)
{
	auto j = json::parse(jsonData);
	auto result = json::object();
	for (auto &v : j)
	{
		std::string varName = v;
		auto d = m_json[ID::variables];
		if (d.contains(varName))
			result[varName] = d[varName];
		else
			result[varName] = nullptr;
	}
	return result.dump();
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

} // namespace NovelTea
