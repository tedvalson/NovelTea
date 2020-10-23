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
	: m_engineVersion(NOVELTEA_VERSION)
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

	_json = json({
		ID::engineVersion, m_engineVersion,
		ID::projectName, "Project Name",
		ID::projectVersion, "1.0",
		ID::projectAuthor, "Author Name",
		ID::projectWebsite, "",
		ID::scriptAfterAction, "",
		ID::scriptBeforeAction, "",
		ID::scriptUndefinedAction, "return false;",
		ID::textFormats, sj::Array(textFormat.toJson())
	});

	_json[ID::projectFonts] = sj::Array(0);
	fromJson(_json);
}

void ProjectData::closeProject()
{
	_json = sj::Array();
	_loaded = false;
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
	std::ofstream file(filename.empty() ? _filename : filename);
	auto j = toJson();
//	json::to_msgpack(j, file);
	file << j;
}

bool ProjectData::loadFromFile(const std::string &filename)
{
	try
	{
		sf::FileInputStream file;
		std::string s;
		if (!file.open(filename))
			return false;

		s.resize(file.getSize());
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
