#include <NovelTea/Object.hpp>
#include <iostream>

namespace NovelTea
{

Object::Object()
	: m_name("New Object")
{
}

Object::~Object()
{
	std::cout << "Object destroyed!" << std::endl;
}

json Object::toJson() const
{
	auto j = json::array({
		m_name
	});
	return j;
}

bool Object::fromJson(const json &j)
{
	m_name = j[0];
	return true;
}

} // namespace NovelTea
