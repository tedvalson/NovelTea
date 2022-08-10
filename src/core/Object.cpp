#include <NovelTea/Object.hpp>

namespace NovelTea
{

Object::Object(Context *context)
	: Entity(context)
	, m_name("Object Name")
	, m_caseSensitive(false)
{
}

size_t Object::jsonSize() const
{
	return 5;
}

json Object::toJson() const
{
	auto j = sj::Array(
		m_id,
		m_parentId,
		m_properties,
		m_name,
		m_caseSensitive
	);
	return j;
}

void Object::loadJson(const json &j)
{
	m_id = j[0].ToString();
	m_parentId = j[1].ToString();
	m_properties = j[2];
	m_name = j[3].ToString();
	m_caseSensitive = j[4].ToBool();
}

EntityType Object::entityType() const
{
	return EntityType::Object;
}

} // namespace NovelTea
