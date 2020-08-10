#include <NovelTea/Room.hpp>
#include <NovelTea/ProjectData.hpp>
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
	return 6;
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
		m_description,
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
	m_description = j[3].ToString();
	m_paths = j[5];
	for (auto &jroomObject : j[4].ArrayRange())
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
			for (auto &jroomObject : jroom[4].ArrayRange())
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
	m_id = idName;
	m_objectList->attach(id, idName);
}

const std::shared_ptr<ObjectList> &Room::getObjectList() const
{
	return m_objectList;
}

} // namespace NovelTea
