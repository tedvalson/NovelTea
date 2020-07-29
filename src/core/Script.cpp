#include <NovelTea/Script.hpp>

namespace NovelTea
{

Script::Script()
	: m_global(false)
	, m_autorun(false)
{
}

size_t Script::jsonSize() const
{
	return 6;
}

json Script::toJson() const
{
	auto j = sj::Array(
		m_id,
		m_parentId,
		m_properties,
		m_global,
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
	m_global = j[3].ToBool();
	m_autorun = j[4].ToBool();
	m_content = j[5].ToString();
}

} // namespace NovelTea
