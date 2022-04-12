#include <NovelTea/Map.hpp>

namespace NovelTea
{

Map::Map()
{
}

size_t Map::jsonSize() const
{
	return 3;
}

json Map::toJson() const
{
	auto j = sj::Array(
		m_id,
		m_parentId,
		m_properties
	);
	return j;
}

void Map::loadJson(const json &j)
{
	m_id = j[0].ToString();
	m_parentId = j[1].ToString();
	m_properties = j[2];
}

EntityType Map::entityType() const
{
	return EntityType::Map;
}

} // namespace NovelTea
