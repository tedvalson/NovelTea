#include <NovelTea/Room.hpp>
#include <iostream>

namespace NovelTea
{

Room::Room()
: m_name("null")
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
}

} // namespace NovelTea
