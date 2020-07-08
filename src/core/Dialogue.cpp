#include <NovelTea/Dialogue.hpp>

namespace NovelTea
{

Dialogue::Dialogue()
: m_name("New Dialogue")
{
}

size_t Dialogue::jsonSize() const
{
	return 3;
}

json Dialogue::toJson() const
{
	auto j = json::array({
		m_id,
		m_parentId,
		m_name,
	});
	return j;
}

void Dialogue::loadJson(const json &j)
{
	m_id = j[0];
	m_parentId = j[1];
	m_name = j[2];
}

} // namespace NovelTea
