#include <NovelTea/MapRoom.hpp>

namespace NovelTea
{

MapRoom::MapRoom()
{
}

json MapRoom::toJson() const
{
	auto j = sj::Array(
		m_name
	);
	return j;
}

bool MapRoom::fromJson(const json &j)
{
	if (!j.IsArray() || j.size() != 1)
		return false;

	m_name = j[0].ToString();
	return true;
}

} // namespace NovelTea
