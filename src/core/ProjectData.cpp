#include <NovelTea/ProjectData.hpp>
#include <NovelTea/ProjectDataIdentifiers.hpp>
#include <NovelTea/Game.hpp>
#include <NovelTea/AssetManager.hpp>
#include <NovelTea/Action.hpp>
#include <NovelTea/Cutscene.hpp>
#include <NovelTea/Dialogue.hpp>
#include <NovelTea/Room.hpp>
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

TextFormat ProjectData::textFormat(size_t index) const
{
	if (index >= m_textFormats.size())
	{
		// TODO: throw error? Return const ref?
		return TextFormat();
	}

	return m_textFormats[index];
}

size_t ProjectData::addTextFormat(const TextFormat &textFormat)
{
	for (size_t i = 0; i < m_textFormats.size(); ++i)
		if (textFormat == m_textFormats[i])
			return i;
	m_textFormats.push_back(textFormat);
	return m_textFormats.size() - 1;
}

bool ProjectData::removeTextFormat(size_t index)
{
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

std::shared_ptr<sf::Font> ProjectData::getFont(size_t index) const
{
	if (index >= m_fonts.size())
		return nullptr;
	return m_fonts[index];
}

void ProjectData::saveToFile(const std::string &filename)
{
	if (filename.empty() && m_filename.empty())
		return;
	if (!filename.empty())
		m_filename = filename;
	std::ofstream file(m_filename);
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
			m_filename = filename;
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
	for (auto &format : m_textFormats)
		jtextformats.append(format.toJson());

	// Cutscene list
//	json jcutscenes = json::array();

	// Project components all together
//	auto jproject = json::object({
//		{"config", jconfig},
//		{"textformats", jtextformats}
//	});

	m_json[ID::textFormats] = jtextformats;

	return m_json;
//	return jproject;
}

bool ProjectData::fromJson(const json &j)
{
	m_loaded = false;
	m_filename.clear();
	m_textFormats.clear();
	m_fonts.clear();

	for (auto &jformat : j[ID::textFormats].ArrayRange())
	{
		TextFormat format;
		format.fromJson(jformat);
		m_textFormats.push_back(format);
	}

	for (auto &jfont : j[ID::projectFonts].ArrayRange())
	{
		auto font = AssetManager<sf::Font>::get("fonts/DejaVuSerif.ttf");
		std::cout << "Loading font: " << jfont.ToString() << std::endl;
		if (font)
			m_fonts.push_back(font);
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
