#include <NovelTea/Script.hpp>

namespace NovelTea
{

Script::Script()
	: m_autorun(false)
{
}

size_t Script::jsonSize() const
{
	return 5;
}

json Script::toJson() const
{
	auto j = sj::Array(
		m_id,
		m_parentId,
		m_properties,
		m_autorun,
		m_content
	);
	return j;
}

void Script::loadJson(const json &j)
{
	m_id = j[0].ToString();
	m_parentId = j[1].ToString();
	m_properties = j[2];
	m_autorun = j[3].ToBool();
	m_content = j[4].ToString();
}

EntityType Script::entityType() const
{
	return EntityType::Script;
}

} // namespace NovelTea
