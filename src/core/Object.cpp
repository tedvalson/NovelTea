#include <NovelTea/Object.hpp>

namespace NovelTea
{

Object::Object()
	: m_name("New Object")
{
}

size_t Object::jsonSize() const
{
	return 4;
}

json Object::toJson() const
{
	auto j = json::array({
		m_id,
		m_parentId,
		m_properties,
		m_name,
	});
	return j;
}

void Object::loadJson(const json &j)
{
	m_id = j[0];
	m_parentId = j[1];
	m_properties = j[2];
	m_name = j[3];
}

} // namespace NovelTea
