#include <NovelTea/Room.hpp>
#include <iostream>

namespace NovelTea
{

Room::Room()
	: m_name("New Room")
{
}

Room::~Room()
{
	std::cout << "room destroyed!" << std::endl;
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
		m_name,
		m_description,
		jobjects,
	});
	return j;
}

bool Room::fromJson(const json &j)
{
	m_objects.clear();
	if (!j.is_array() || j.size() != 3)
		return false;

	try
	{
		m_name = j[0];
		m_description = j[1];
		for (auto &jroomObject : j[2])
			m_objects.push_back({jroomObject[0], jroomObject[1]});
		return true;
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return false;
	}
}

} // namespace NovelTea
