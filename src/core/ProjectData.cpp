#include <NovelTea/ProjectData.hpp>
#include <NovelTea/AssetManager.hpp>
#include <NovelTea/Action.hpp>
#include <NovelTea/Cutscene.hpp>
#include <NovelTea/Object.hpp>
#include <NovelTea/Room.hpp>
#include <NovelTea/Verb.hpp>
#include <fstream>
#include <iostream>

namespace NovelTea
{

ProjectData::ProjectData()
{
}

ProjectData::~ProjectData()
{

}

ProjectData &ProjectData::instance()
{
	static ProjectData obj;
	return obj;
}

void ProjectData::newProject()
{
	Cutscene cutscene;
	TextFormat textFormat;

	_json = json::object({
		{ID::engineVersion, m_engineVersion},
		{ID::projectName, "Project Name"},
		{ID::projectVersion, "1.0"},
		{ID::projectAuthor, "Author Name"},
		{ID::projectWebsite, ""},
		{ID::textFormats, json::array({textFormat})},
	});

	_json[ID::cutscenes]["New Cutscene"] = cutscene;
	_json[ID::projectFonts] = json::array({0});
	fromJson(_json);
}

void ProjectData::closeProject()
{
	_json = json::array();
	_loaded = false;
}

bool ProjectData::isLoaded() const
{
	return _loaded;
}

bool ProjectData::isValid(std::string &errorMessage) const
{
	auto entryPoint = data().value(ID::projectEntrypoint, json::array({-1,""}));
//	auto entryIdName = entryPoint.value(NT_ENTITY_ID, "");
	if (entryPoint[1].empty())
	{
		errorMessage = "No valid entry point defined in project settings.";
		return false;
	}

	return true;
}

TextFormat ProjectData::textFormat(size_t index) const
{
	if (index >= _textFormats.size())
	{
		// TODO: throw error? Return const ref?
		return TextFormat();
	}

	return _textFormats[index];
}

size_t ProjectData::addTextFormat(const TextFormat &textFormat)
{
	for (size_t i = 0; i < _textFormats.size(); ++i)
		if (textFormat == _textFormats[i])
			return i;
	_textFormats.push_back(textFormat);
	return _textFormats.size() - 1;
}

bool ProjectData::removeTextFormat(size_t index)
{
	return true;
}

std::shared_ptr<sf::Font> ProjectData::getFont(size_t index) const
{
	if (index >= m_fonts.size())
		return nullptr;
	return m_fonts[index];
}

std::shared_ptr<Action> ProjectData::action(const std::string &idName)
{
	if (!_json[ID::actions].contains(idName))
		return nullptr;
	return std::make_shared<Action>(_json[ID::actions][idName].get<Action>());
}

std::shared_ptr<Cutscene> ProjectData::cutscene(const std::string &idName)
{
	if (!_json[ID::cutscenes].contains(idName))
		return nullptr;
	return std::make_shared<Cutscene>(_json[ID::cutscenes][idName].get<Cutscene>());
}

std::shared_ptr<Room> ProjectData::room(const std::string &idName)
{
	if (!_json[ID::rooms].contains(idName))
		return nullptr;
	return std::make_shared<Room>(_json[ID::rooms][idName].get<Room>());
}

std::shared_ptr<Object> ProjectData::object(const std::string &idName)
{
	if (!_json[ID::objects].contains(idName))
		return nullptr;
	return std::make_shared<Object>(_json[ID::objects][idName].get<Object>());
}

std::shared_ptr<Verb> ProjectData::verb(const std::string &idName)
{
	if (!_json[ID::verbs].contains(idName))
		return nullptr;
	return std::make_shared<Verb>(_json[ID::verbs][idName].get<Verb>());
}

void ProjectData::saveToFile(const std::string &filename)
{
	if (filename.empty() && _filename.empty())
		return;
	std::ofstream file(filename.empty() ? _filename : filename);
	auto j = toJson();
//	json::to_msgpack(j, file);
	file << j;
}

bool ProjectData::loadFromFile(const std::string &filename)
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
		std::cout << "Failed to load project: " << filename << std::endl;
		std::cout << e.what() << std::endl;
		return false;
	}
}

const std::string &ProjectData::filename() const
{
	return _filename;
}

json ProjectData::toJson() const
{
//	json jconfig = json::object({
//		{"engine", m_engineVersion},
//		{"author", "Tom2"},
//		{"startpoint", {
//			{"type", "cutscene"},
//			{"id", 0}
//		}}
//	});

	// TextFormat list
	json jtextformats = json::array();
	for (auto &format : _textFormats)
		jtextformats.push_back(format);

	// Cutscene list
//	json jcutscenes = json::array();

	// Project components all together
//	auto jproject = json::object({
//		{"config", jconfig},
//		{"textformats", jtextformats}
//	});

	_json[ID::engineVersion] = m_engineVersion;
	_json[ID::textFormats] = jtextformats;

	return _json;
//	return jproject;
}

bool ProjectData::fromJson(const json &j)
{
	_loaded = false;
	_filename.clear();
	_textFormats.clear();
	m_fonts.clear();

	for (auto &jformat : j[ID::textFormats])
		_textFormats.push_back(jformat);

	for (auto &jfont : j[ID::projectFonts])
	{
		auto font = AssetManager<sf::Font>::get("fonts/DejaVuSerif.ttf");
		std::cout << "Loading font: " << jfont << std::endl;
		if (font)
			m_fonts.push_back(font);
	}

	_json = j;
	_loaded = true;
	return true;
}

const json &ProjectData::data() const
{
	return _json;
}

json &ProjectData::data()
{
	return _json;
}

} // namespace NovelTea
