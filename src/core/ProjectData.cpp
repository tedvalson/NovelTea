#include <NovelTea/ProjectData.hpp>
#include <NovelTea/Cutscene.hpp>
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

	_json = json::object({
		{NT_ENGINE_VERSION, m_engineVersion},
		{NT_PROJECT_NAME, "Project Name"},
		{NT_PROJECT_VERSION, "1.0"},
		{NT_PROJECT_AUTHOR, "Author Name"},
		{NT_PROJECT_WEBSITE, ""},
		{NT_TEXTFORMATS, json::array({{
			false, false, false, 12, {0,0,0,255}
		}})},
//		{NT_CUTSCENES,
//			{"New", cutscene}
//			{"0", json::array({"Test scene",{0,"Text"}})},
//		},
		{"startpoint", {
			{"type", "cutscene"},
			{"id", 0}
		}}
	});

	_json[NT_CUTSCENES]["New Cutscene"] = cutscene;
	_json[NT_PROJECT_FONTS] = json::array({0});
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
	auto entryPoint = data().value(NT_PROJECT_ENTRYPOINT, json::object());
	auto entryIdName = entryPoint.value(NT_ENTITY_ID, "");
	if (entryIdName.empty())
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

std::shared_ptr<Cutscene> ProjectData::cutscene(const std::string &idName)
{
	if (!_json[NT_CUTSCENES].contains(idName))
		return nullptr;
	return std::make_shared<Cutscene>(_json[NT_CUTSCENES][idName].get<Cutscene>());
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
	json jconfig = json::object({
		{"engine", m_engineVersion},
		{"author", "Tom2"},
		{"startpoint", {
			{"type", "cutscene"},
			{"id", 0}
		}}
	});

	// TextFormat list
	json jtextformats = json::array();
	for (auto &format : _textFormats)
		jtextformats.push_back(format);

	// Cutscene list
	json jcutscenes = json::array();

	// Project components all together
	auto jproject = json::object({
		{"config", jconfig},
		{"textformats", jtextformats}
	});

	_json[NT_TEXTFORMATS] = jtextformats;

	return _json;
//	return jproject;
}

bool ProjectData::fromJson(const json &j)
{
	_loaded = false;
	_filename.clear();
	_textFormats.clear();
	m_fonts.clear();

	for (auto &jformat : j[NT_TEXTFORMATS])
		_textFormats.push_back(jformat);

	for (auto &jfont : j[NT_PROJECT_FONTS])
	{
		auto font = std::make_shared<sf::Font>();
		std::cout << "Loading font: " << jfont << std::endl;
		if (font->loadFromFile("/home/android/dev/NovelTea/res/fonts/DejaVuSans.ttf"))
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
