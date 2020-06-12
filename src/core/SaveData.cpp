#include <NovelTea/SaveData.hpp>
#include <NovelTea/ProjectDataIdentifiers.hpp>
#include <fstream>
#include <iostream>

namespace NovelTea
{

SaveData::SaveData()
{
}

SaveData::~SaveData()
{

}

SaveData &SaveData::instance()
{
	static SaveData obj;
	return obj;
}

bool SaveData::isLoaded() const
{
	return _loaded;
}

void SaveData::saveToFile(const std::string &filename)
{
	if (filename.empty() && _filename.empty())
		return;
	std::ofstream file(filename.empty() ? _filename : filename);
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
			_filename = filename;
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
	return _filename;
}

json SaveData::toJson() const
{
//	_json[NT_ENGINE_VERSION] = m_engineVersion;
	return _json;
}

bool SaveData::fromJson(const json &j)
{
	_loaded = false;
	_filename.clear();

	_json = j;
	_loaded = true;
	return true;
}

const json &SaveData::data() const
{
	return _json;
}

json &SaveData::data()
{
	return _json;
}

void SaveData::saveVariables(const std::string &jsonData)
{
	auto j = json::parse(jsonData);
	for (auto &item : j.items())
	{
		Save.data()[ID::variables][item.key()] = item.value();
	}
}

std::string SaveData::loadVariables(const std::string &jsonData)
{
	auto j = json::parse(jsonData);
	auto result = json::object();
	for (auto &v : j)
	{
		std::string varName = v;
		auto d = Save.data()[ID::variables];
		if (d.contains(varName))
			result[varName] = d[varName];
		else
			result[varName] = nullptr;
	}
	return result.dump();
}

} // namespace NovelTea
