#include <NovelTea/ProjectData.hpp>
#include <NovelTea/ProjectDataIdentifiers.hpp>
#include <NovelTea/Game.hpp>
#include <NovelTea/AssetManager.hpp>
#include <NovelTea/Action.hpp>
#include <SFML/System/FileInputStream.hpp>
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
	TextFormat textFormat;

	auto j = json({
		ID::engineVersion, NOVELTEA_VERSION,
		ID::projectName, "Project Name",
		ID::projectVersion, "1.0",
		ID::projectAuthor, "Author Name",
		ID::projectWebsite, "",
		ID::startingInventory, sj::Array(),
		ID::scriptAfterAction, "",
		ID::scriptBeforeAction, "return true;",
		ID::scriptUndefinedAction, "return false;",
		ID::scriptAfterLeave, "",
		ID::scriptBeforeLeave, "return true;",
		ID::scriptAfterEnter, "",
		ID::scriptBeforeEnter, "return true;",
		ID::textFormats, sj::Array(textFormat.toJson()),
		ID::openTabs, sj::Array(),
		ID::openTabIndex, -1,
	});

	j[ID::projectFonts] = sj::Array("fontawesome");
	fromJson(j);
}

void ProjectData::closeProject()
{
	_json = sj::Array();
	_loaded = false;
	_filename.clear();
}

void ProjectData::clearFilename()
{
	_filename.clear();
}

bool ProjectData::isLoaded() const
{
	return _loaded;
}

bool ProjectData::isValid(std::string &errorMessage) const
{
	auto entryPoint = sj::Array(-1, "");
	if (_json.hasKey(ID::entrypointEntity))
		entryPoint = _json[ID::entrypointEntity];
//	auto entryIdName = entryPoint.value(NT_ENTITY_ID, "");
	if (entryPoint[1].IsEmpty())
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

void ProjectData::saveToFile(const std::string &filename)
{
	if (filename.empty() && _filename.empty())
		return;
	if (!filename.empty())
		_filename = filename;
	std::ofstream file(_filename);
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

		std::string s;
		file.seekg(0, std::ios_base::end);
		s.resize(file.tellg());
		file.seekg(0);
		file.read(&s[0], s.size());

//		auto j = json::from_msgpack(file);
//		auto j = json::parse(file);

		auto j = json::Load(s);
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
	json jtextformats = sj::Array();
	for (auto &format : _textFormats)
		jtextformats.append(format.toJson());

	// Cutscene list
//	json jcutscenes = json::array();

	// Project components all together
//	auto jproject = json::object({
//		{"config", jconfig},
//		{"textformats", jtextformats}
//	});

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

	for (auto &jformat : j[ID::textFormats].ArrayRange())
	{
		TextFormat format;
		format.fromJson(jformat);
		_textFormats.push_back(format);
	}

	for (auto &jfont : j[ID::projectFonts].ArrayRange())
	{
		auto font = AssetManager<sf::Font>::get("fonts/DejaVuSerif.ttf");
		std::cout << "Loading font: " << jfont.ToString() << std::endl;
		if (font)
			m_fonts.push_back(font);
	}

	GMan; // Make sure GameManager is initialized
	_json = j;
	_loaded = true;
	ActiveGame->reset();

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
