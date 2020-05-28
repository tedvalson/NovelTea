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
	auto j = json::array({
	});
	return j;
}

bool Room::fromJson(const json &j)
{
	return true;
}

} // namespace NovelTea
