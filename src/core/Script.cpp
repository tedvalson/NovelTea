#include <NovelTea/Script.hpp>

namespace NovelTea
{

Script::Script()
{
}

size_t Script::jsonSize() const
{
	return 3;
}

json Script::toJson() const
{
	auto j = json::array({
		m_id,
		m_parentId,
		m_content,
	});
	return j;
}

void Script::loadJson(const json &j)
{
	m_id = j[0];
	m_parentId = j[1];
	m_content = j[2];
}

} // namespace NovelTea
