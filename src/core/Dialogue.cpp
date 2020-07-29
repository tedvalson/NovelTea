#include <NovelTea/Dialogue.hpp>

namespace NovelTea
{

Dialogue::Dialogue()
: m_name("New Dialogue")
{
}

size_t Dialogue::jsonSize() const
{
	return 4;
}

json Dialogue::toJson() const
{
	auto j = sj::Array(
		m_id,
		m_parentId,
		m_properties,
		m_name
	);
	return j;
}

void Dialogue::loadJson(const json &j)
{
	m_id = j[0].ToString();
	m_parentId = j[1].ToString();
	m_properties = j[2];
	m_name = j[3].ToString();
}

} // namespace NovelTea
