#include <NovelTea/Object.hpp>
#include <iostream>

namespace NovelTea
{

Object::Object()
	: m_name("New Object")
{
}

json Object::toJson() const
{
	auto j = json::array({
		m_id,
		m_name,
	});
	return j;
}

bool Object::fromJson(const json &j)
{
	if (!j.is_array() || j.size() != 2)
		return false;

	try
	{
		m_id = j[0];
		m_name = j[1];
		return true;
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return false;
	}
}

} // namespace NovelTea
