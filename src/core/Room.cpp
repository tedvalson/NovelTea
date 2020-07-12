#include <NovelTea/Room.hpp>
#include <NovelTea/ProjectData.hpp>
#include <iostream>

namespace NovelTea
{

Room::Room()
: m_name("null")
, m_objectList(std::make_shared<ObjectList>())
{
}

Room::~Room()
{
	std::cout << "room destroyed!" << std::endl;
}

size_t Room::jsonSize() const
{
	return 5;
}

json Room::toJson() const
{
	auto jobjects = json::array();
	for (auto &roomObject : m_objects)
	{
		auto jobject = json::array({
			roomObject.idName,
			roomObject.placeInRoom
		});
		jobjects.push_back(jobject);
	}
	auto j = json::array({
		m_id,
		m_parentId,
		m_name,
		m_description,
		jobjects,
	});
	return j;
}

void Room::loadJson(const json &j)
{
	m_id = j[0];
	m_parentId = j[1];
	m_name = j[2];
	m_description = j[3];
	for (auto &jroomObject : j[4])
		m_objects.push_back({jroomObject[0], jroomObject[1]});

	m_objectList->attach(id, m_id);
}

bool Room::contains(const std::string &objectId)
{
	return m_objectList->contains(objectId);
}

json Room::getProjectRoomObjects()
{
	auto j = json::object();
	if (Proj.isLoaded())
	{
		for (auto &item : ProjData[id].items())
		{
			auto jroom = item.value();
			auto jobjects = json::array();
			for (auto &jroomObject : jroom[4])
			{
				bool placeInRoom = jroomObject[1];
				if (placeInRoom)
					jobjects.push_back(jroomObject[0]);
			}
			j[item.key()] = jobjects;
		}
	}
	return j;
}

void Room::setId(const std::string &idName)
{
	m_id = idName;
	m_objectList->attach(id, idName);
}

const std::shared_ptr<ObjectList> &Room::getObjectList() const
{
	return m_objectList;
}

} // namespace NovelTea
