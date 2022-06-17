#include <NovelTea/ProjectData.hpp>
#include <NovelTea/ProjectDataIdentifiers.hpp>
#include <NovelTea/Game.hpp>
#include <NovelTea/AssetManager.hpp>
#include <NovelTea/Action.hpp>
#include <NovelTea/Cutscene.hpp>
#include <NovelTea/Dialogue.hpp>
#include <NovelTea/Room.hpp>
#include <NovelTea/StringUtils.hpp>
#include <NovelTea/Zip/Zip.hpp>
#include <SFML/System/FileInputStream.hpp>
#include <fstream>
#include <iostream>

namespace NovelTea
{

ProjectData::ProjectData()
: m_loaded(false)
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
	auto j = json({
		ID::engineVersion, NOVELTEA_VERSION,
		ID::projectName, "Project Name",
		ID::projectVersion, "1.0",
		ID::projectAuthor, "Author Name",
		ID::projectWebsite, "",
		ID::projectFontDefault, "sys",
		ID::projectFonts, sj::Array(),
		ID::startingInventory, sj::Array(),
		ID::scriptBeforeSave, "",
		ID::scriptAfterLoad, "",
		ID::scriptAfterAction, "",
		ID::scriptBeforeAction, "return true;",
		ID::scriptUndefinedAction, "return false;",
		ID::scriptAfterLeave, "",
		ID::scriptBeforeLeave, "return true;",
		ID::scriptAfterEnter, "",
		ID::scriptBeforeEnter, "return true;",
		ID::openTabs, sj::Array(),
		ID::openTabIndex, -1,
	});

	j[ID::engineFonts] = json({
	   "sys", "LiberationSans",
	   "sysIcon", "fontawesome",
   });
	fromJson(j);
}

void ProjectData::closeProject()
{
	m_json = sj::Array();
	m_loaded = false;
	m_filename.clear();
}

void ProjectData::clearFilename()
{
	m_filename.clear();
}

bool ProjectData::isLoaded() const
{
	return m_loaded;
}

bool ProjectData::isValid(std::string &errorMessage) const
{
	auto entryPoint = sj::Array(-1, "");
	if (m_json.hasKey(ID::entrypointEntity))
		entryPoint = m_json[ID::entrypointEntity];
//	auto entryIdName = entryPoint.value(NT_ENTITY_ID, "");
	if (entryPoint[1].IsEmpty())
	{
		errorMessage = "No valid entry point defined in project settings.";
		return false;
	}

	return true;
}

void renameJsonEntity(json &jentity, EntityType entityType, const std::string &oldName, const std::string &newName)
{
	if (jentity[0].ToInt() == static_cast<int>(entityType) && jentity[1].ToString() == oldName)
		jentity[1] = newName;
}

void ProjectData::renameEntity(EntityType entityType, const std::string &oldName, const std::string &newName)
{
	if (oldName == newName || entityType == EntityType::Invalid)
		return;
	auto entityId = entityTypeToId(entityType);

	// Perform the actual renaming
	auto &j = m_json[entityId];
	j[newName] = j[oldName];
	j[newName][0] = newName;
	j.erase(oldName);

	// Rename parent refs
	for (auto &entityPair : j.ObjectRange())
	{
		auto &jparentId = entityPair.second[1];
		if (jparentId.ToString() == oldName)
			jparentId = newName;
	}

	// Project settings
	renameJsonEntity(m_json[ID::entrypointEntity], entityType, oldName, newName);

	// Actions
	if (entityType == EntityType::Object || entityType == EntityType::Verb)
	{
		auto action = std::make_shared<Action>();
		for (auto &actionPair : m_json[Action::id].ObjectRange())
		{
			action->fromJson(actionPair.second);
			auto jcombo = action->getVerbObjectCombo();
			if (entityType == EntityType::Verb && jcombo[0].ToString() == oldName)
				jcombo[0] = newName;
			else if (entityType == EntityType::Object)
				for (auto &jobject : jcombo[1].ArrayRange())
					if (jobject.ToString() == oldName)
						jobject = newName;
			action->setVerbObjectCombo(jcombo);
			m_json[Action::id][actionPair.first] = action->toJson();
		}
	}

	// Cutscenes
	auto cutscene = std::make_shared<Cutscene>();
	for (auto &pair : m_json[Cutscene::id].ObjectRange())
	{
		cutscene->fromJson(pair.second);
		auto jentity = cutscene->getNextEntityJson();
		renameJsonEntity(jentity, entityType, oldName, newName);
		cutscene->setNextEntityJson(jentity);
		m_json[Cutscene::id][pair.first] = cutscene->toJson();
	}

	// Dialogues
	auto dialogue = std::make_shared<Dialogue>();
	for (auto &pair : m_json[Dialogue::id].ObjectRange())
	{
		dialogue->fromJson(pair.second);
		auto jentity = dialogue->getNextEntityJson();
		renameJsonEntity(jentity, entityType, oldName, newName);
		dialogue->setNextEntityJson(jentity);
		m_json[Dialogue::id][pair.first] = dialogue->toJson();
	}

	// Rooms
	auto room = std::make_shared<Room>();
	for (auto &roomPair : m_json[Room::id].ObjectRange())
	{
		room->fromJson(roomPair.second);

		if (entityType == EntityType::Object)
		{
			auto objects = room->getObjects();
			for (auto &object : objects)
				if (object.idName == oldName)
					object.idName = newName;
			room->setObjects(objects);

			// Rename object refs in room description
			auto d = room->getDescriptionRaw();
			room->setDescriptionRaw(replace(d, "|"+oldName+"]]", "|"+newName+"]]"));
		}
		else
		{
			auto paths = room->getPaths();
			for (auto &jpath : paths.ArrayRange())
				renameJsonEntity(jpath[1], entityType, oldName, newName);
			room->setPaths(paths);
		}

		m_json[Room::id][roomPair.first] = room->toJson();
	}

	// Tests
	for (auto &pair : m_json[ID::tests].ObjectRange())
	{
		auto &j = pair.second;
		renameJsonEntity(j[ID::entrypointEntity], entityType, oldName, newName);
		if (entityType == EntityType::Object)
			for (auto &jobjectId : j[ID::startingInventory].ArrayRange())
				if (jobjectId.ToString() == oldName)
					jobjectId = newName;
		if (entityType == EntityType::Object || entityType == EntityType::Verb)
			for (auto &jstep : j[ID::testSteps].ArrayRange())
				if (jstep["type"].ToString() == "action")
				{
					if (entityType == EntityType::Object) {
						for (auto &jobjectId : jstep["objects"].ArrayRange())
							if (jobjectId.ToString() == oldName)
								jobjectId = newName;
					} else {
						auto &verbId = jstep["verb"];
						if (verbId.ToString() == oldName)
							verbId = newName;
					}
				}
	}
}

void ProjectData::setFontData(const std::string &alias, const std::string &data)
{
	m_fontsData[alias] = data;
}

const std::string &ProjectData::getFontData(const std::string &alias) const
{
	return m_fontsData.at(alias);
}

std::shared_ptr<sf::Font> ProjectData::getFont(const std::string &fontName) const
{
	if (m_fonts.find(fontName) == m_fonts.end())
		return m_fonts.at("sys");
	return m_fonts.at(fontName);
}

void ProjectData::saveToFile(const std::string &filename)
{
	if (filename.empty() && m_filename.empty())
		return;
	if (!filename.empty())
		m_filename = filename;

	auto j = toJson();
//	json::to_msgpack(j, file);
	ZipWriter zip(m_filename);
	zip.write("game", j.dump());
	zip.write("image", m_imageData);

	for (auto& jfont : j[ID::projectFonts].ObjectRange()) {
		zip.write("fonts/" + jfont.second.ToString(), m_fontsData[jfont.first]);
	}
}

bool ProjectData::loadFromFile(const std::string &filename)
{
	try
	{
		std::string s;

		sf::FileInputStream file;
		if (!file.open(filename))
			return false;
		s.resize(file.getSize());
		file.read(&s[0], s.size());

		ZipReader zip(s.data(), s.size());

//		auto j = json::from_msgpack(file);
//		auto j = json::parse(file);

		auto j = json::Load(zip.read("game"));
		auto success = fromJson(j);
		if (success) {
			for (auto& jfont : j[ID::projectFonts].ObjectRange()) {
				auto& alias = jfont.first;
				auto font = std::make_shared<sf::Font>();
				m_fontsData[alias] = zip.read("fonts/" + jfont.second.ToString());
				auto &data = m_fontsData[alias];
				if (font->loadFromMemory(data.data(), data.size()))
					m_fonts[alias] = font;
				else {
					std::cerr << "Failed to load project font: " << jfont.second.ToString() << std::endl;
					auto &defaultFont = m_json[ID::projectFontDefault];
					if (alias == defaultFont.ToString())
						defaultFont = "sys";
				}
			}
			m_imageData = zip.read("image");
			m_filename = filename;
		}
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
	return m_filename;
}

void ProjectData::setImageData(const std::string &data)
{
	m_imageData = data;
}

const std::string &ProjectData::getImageData() const
{
	return m_imageData;
}

json ProjectData::toJson() const
{
	// TextFormat list

	// Project components all together
//	auto jproject = json::object({
//		{"config", jconfig},
//		{"textformats", jtextformats}
//	});

	return m_json;
}

bool ProjectData::fromJson(const json &j)
{
	m_loaded = false;
	m_filename.clear();
	m_fonts.clear();
	m_imageData.clear();

	for (auto &jfont : j[ID::engineFonts].ObjectRange())
	{
		auto font = AssetManager<sf::Font>::get("fonts/" + jfont.second.ToString());
		std::cout << "Loading font: " << jfont.second.ToString() << std::endl;
		if (font)
			m_fonts[jfont.first] = font;
	}

	GMan; // Make sure GameManager is initialized
	m_json = j;
	m_loaded = true;
	if (ActiveGame == GMan.getDefault())
		ActiveGame->initialize();
	else
		ActiveGame->reset();

	return true;
}

const json &ProjectData::data() const
{
	return m_json;
}

json &ProjectData::data()
{
	return m_json;
}

} // namespace NovelTea
