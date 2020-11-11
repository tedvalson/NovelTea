#include <NovelTea/Room.hpp>
#include <NovelTea/ProjectData.hpp>
#include <NovelTea/ProjectDataIdentifiers.hpp>
#include <iostream>

namespace NovelTea
{

Room::Room()
: m_objectList(std::make_shared<ObjectList>())
{
	m_paths = sj::Array();
	for (int i = 0; i < 8; ++i)
		m_paths.append(sj::Array(false, sj::Array(-1, "")));
}

Room::~Room()
{
	std::cout << "room destroyed!" << std::endl;
}

size_t Room::jsonSize() const
{
	return 10;
}

json Room::toJson() const
{
	auto jobjects = sj::Array();
	for (auto &roomObject : m_objects)
	{
		auto jobject = sj::Array(
			roomObject.idName,
			roomObject.placeInRoom
		);
		jobjects.append(jobject);
	}
	auto j = sj::Array(
		m_id,
		m_parentId,
		m_properties,
		m_descriptionRaw,
		m_scriptBeforeEnter,
		m_scriptAfterEnter,
		m_scriptBeforeLeave,
		m_scriptAfterLeave,
		jobjects,
		m_paths
	);
	return j;
}

void Room::loadJson(const json &j)
{
	m_id = j[0].ToString();
	m_parentId = j[1].ToString();
	m_properties = j[2];
	m_descriptionRaw = j[3].ToString();
	m_scriptBeforeEnter = j[4].ToString();
	m_scriptAfterEnter = j[5].ToString();
	m_scriptBeforeLeave = j[6].ToString();
	m_scriptAfterLeave = j[7].ToString();
	m_paths = j[9];
	for (auto &jroomObject : j[8].ArrayRange())
		m_objects.push_back({jroomObject[0].ToString(), jroomObject[1].ToBool()});

	m_objectList->attach(id, m_id);
}

bool Room::contains(const std::shared_ptr<Object> &object)
{
	return m_objectList->contains(object);
}

bool Room::containsId(const std::string &objectId)
{
	return m_objectList->containsId(objectId);
}

json Room::getProjectRoomObjects()
{
	auto j = sj::Object();
	if (Proj.isLoaded())
	{
		for (auto &item : ProjData[id].ObjectRange())
		{
			auto jroom = item.second;
			auto jobjects = sj::Array();
			for (auto &jroomObject : jroom[8].ArrayRange())
			{
				bool placeInRoom = jroomObject[1].ToBool();
				if (placeInRoom)
					jobjects.append(jroomObject[0].ToString());
			}
			j[item.first] = jobjects;
		}
	}
	return j;
}

void Room::setId(const std::string &idName)
{
	Entity::setId(idName);
	m_objectList->attach(id, idName);
}

const std::shared_ptr<ObjectList> &Room::getObjectList() const
{
	return m_objectList;
}

std::string Room::getDescription() const
{
	try {
		auto room = ActiveGame->getSaveData().get<Room>(m_id);
		auto script = "function _f(room){\n"+m_descriptionRaw+"\nreturn \"\";}";
		auto newDescription = ActiveGame->getScriptManager().call<std::string>(script, "_f", room);
		return GSave.roomDescription(m_id, newDescription);
	} catch (std::exception &e) {
		std::cerr << "Room::getDescription " << e.what() << std::endl;
		return "You encountered a game error.\n" + std::string(e.what());
	}
}

bool Room::runScriptBeforeEnter() const
{
	auto script = ProjData[ID::scriptBeforeEnter].ToString();
	if (!script.empty() && !ActiveGame->getScriptManager().runRoomScript(m_id, script))
		return false;
	if (m_scriptBeforeEnter.empty())
		return true;
	return ActiveGame->getScriptManager().runRoomScript(m_id, m_scriptBeforeEnter);
}

void Room::runScriptAfterEnter() const
{
	auto script = ProjData[ID::scriptAfterEnter].ToString();
	if (!script.empty())
		ActiveGame->getScriptManager().runRoomScript(m_id, script);
	if (!m_scriptAfterEnter.empty())
		ActiveGame->getScriptManager().runRoomScript(m_id, m_scriptAfterEnter);
}

bool Room::runScriptBeforeLeave() const
{
	auto script = ProjData[ID::scriptBeforeLeave].ToString();
	if (!script.empty() && !ActiveGame->getScriptManager().runRoomScript(m_id, script))
		return false;
	if (m_scriptBeforeLeave.empty())
		return true;
	return ActiveGame->getScriptManager().runRoomScript(m_id, m_scriptBeforeLeave);
}

void Room::runScriptAfterLeave() const
{
	auto script = ProjData[ID::scriptAfterLeave].ToString();
	if (!script.empty())
		ActiveGame->getScriptManager().runRoomScript(m_id, script);
	if (!m_scriptAfterLeave.empty())
		ActiveGame->getScriptManager().runRoomScript(m_id, m_scriptAfterLeave);
}

} // namespace NovelTea
