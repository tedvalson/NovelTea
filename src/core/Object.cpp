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
	});
	return j;
}

bool Object::fromJson(const json &j)
{
	return true;
}

} // namespace NovelTea
