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
	auto j = json::array({
		m_id,
		m_parentId,
		m_properties,
		m_global,
		m_autorun,
		m_content,
	});
	return j;
}

void Script::loadJson(const json &j)
{
	m_id = j[0];
	m_parentId = j[1];
	m_properties = j[2];
	m_global = j[3];
	m_autorun = j[4];
	m_content = j[5];
}

} // namespace NovelTea
