#include <NovelTea/Room.hpp>
#include <NovelTea/Context.hpp>
#include <NovelTea/ProjectDataIdentifiers.hpp>
#include <NovelTea/SaveData.hpp>
#include <NovelTea/ScriptManager.hpp>
#include <NovelTea/PropertyList.hpp>
#include <iostream>

namespace NovelTea
{

Room::Room(Context *context)
	: Entity(context)
	, m_objectList(std::make_shared<ObjectList>(context))
{
	m_paths = sj::Array();
	for (int i = 0; i < 8; ++i)
		m_paths.append(sj::Array(false, sj::Array(-1, "")));
}

size_t Room::jsonSize() const
{
	return 11;
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
		m_paths,
		m_name
	);
	return j;
}

void Room::loadJson(const json &j)
{
	m_objects.clear();
	m_id = j[0].ToString();
	m_parentId = j[1].ToString();
	m_properties = j[2];
	m_descriptionRaw = j[3].ToString();
	m_scriptBeforeEnter = j[4].ToString();
	m_scriptAfterEnter = j[5].ToString();
	m_scriptBeforeLeave = j[6].ToString();
	m_scriptAfterLeave = j[7].ToString();
	m_paths = j[9];
	m_name = j[10].ToString();
	for (auto &jroomObject : j[8].ArrayRange())
		m_objects.push_back({jroomObject[0].ToString(), jroomObject[1].ToBool()});

	m_objectList->attach(id, m_id);
}

EntityType Room::entityType() const
{
	return EntityType::Room;
}

bool Room::contains(const std::shared_ptr<Object> &object)
{
	return m_objectList->contains(object);
}

bool Room::containsId(const std::string &objectId)
{
	return m_objectList->containsId(objectId);
}

json Room::getProjectRoomObjects(Context *context)
{
	auto j = sj::Object();
	auto proj = context->getGame()->getProjectData();
	if (proj->isLoaded())
	{
		for (auto &item : proj->data()[id].ObjectRange())
		{
			auto jroom = item.second;
			auto jobjects = sj::Array();
			for (auto &jroomObject : jroom[8].ArrayRange())
			{
				bool placeInRoom = jroomObject[1].ToBool();
				if (placeInRoom)
					jobjects.append(sj::Array(jroomObject[0].ToString(), 1));
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

void Room::setVisitCount(int count)
{
	auto& j = GSave->data()[ID::visitedRooms];
	if (count <= 0)
		j.erase(getId());
	j[getId()] = count;
}

int Room::getVisitCount() const
{
	auto& j = GSave->data()[ID::visitedRooms];
	if (!j.hasKey(getId()))
		return 0;
	return j[getId()].ToInt();
}

void Room::setDescription(const std::string &description)
{
	// Use JSON object as a lazy way to escape the string
	sj::JSON j = description;
	m_descriptionRaw = "text=" + j.dump();
}

std::string Room::getDescription() const
{
	try {
		auto room = GGame->get<Room>(m_id);
		ScriptMan->setActiveEntity(room);
		auto script = "var text='';\n" + m_descriptionRaw + "\nreturn text;";
		auto newDescription = ScriptMan->runInClosure<std::string>(script);
		return GSave->roomDescription(m_id, newDescription);
	} catch (std::exception &e) {
		std::cerr << "Room::getDescription " << e.what() << std::endl;
		return "You encountered a game error.\n" + std::string(e.what());
	}
}

// Sync to changes in SaveData
void Room::sync()
{
	m_objectList->sync();
	getPropertyList()->sync();
}

bool Room::runScript(const std::string &script) const
{
	auto room = GGame->get<Room>(m_id);
	auto s = "function _f(room){\n"+script+"\nreturn true;}";
	ScriptMan->setActiveEntity(room);
	try {
		return ScriptMan->call<bool>(s, "_f", room);
	} catch (std::exception &e) {
		std::cerr << "runRoomScript (" << m_id << ") " << e.what() << std::endl;
		return false;
	}
}

bool Room::runScriptBeforeEnter() const
{
	auto script = ProjData[ID::scriptBeforeEnter].ToString();
	if (!script.empty() && !runScript(script))
		return false;
	if (m_scriptBeforeEnter.empty())
		return true;
	return runScript(m_scriptBeforeEnter);
}

void Room::runScriptAfterEnter() const
{
	auto script = ProjData[ID::scriptAfterEnter].ToString();
	if (!script.empty())
		runScript(script);
	if (!m_scriptAfterEnter.empty())
		runScript(m_scriptAfterEnter);
}

bool Room::runScriptBeforeLeave() const
{
	auto script = ProjData[ID::scriptBeforeLeave].ToString();
	if (!script.empty() && !runScript(script))
		return false;
	if (m_scriptBeforeLeave.empty())
		return true;
	return runScript(m_scriptBeforeLeave);
}

void Room::runScriptAfterLeave() const
{
	auto script = ProjData[ID::scriptAfterLeave].ToString();
	if (!script.empty())
		runScript(script);
	if (!m_scriptAfterLeave.empty())
		runScript(m_scriptAfterLeave);
}

void Room::setPathEntity(int direction, std::shared_ptr<Entity> entity)
{
	m_paths[direction] = sj::Array(true, sj::Array(static_cast<int>(entity->entityType()), entity->getId()));
}

void Room::setPathScript(int direction, const std::string &script)
{
	m_paths[direction] = sj::Array(true, sj::Array(static_cast<int>(EntityType::CustomScript), script));
}

void Room::enablePath(int direction)
{
	m_paths[direction][0] = true;
}

void Room::disablePath(int direction)
{
	m_paths[direction][0] = false;
}

} // namespace NovelTea
