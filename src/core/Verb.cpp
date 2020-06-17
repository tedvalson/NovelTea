#include <NovelTea/Verb.hpp>
#include <iostream>

namespace NovelTea
{

Verb::Verb()
	: m_name("New Verb")
{
}

json Verb::toJson() const
{
	auto j = json::array({
		m_name,
	});
	return j;
}

bool Verb::fromJson(const json &j)
{
	m_name = j[0];
	return true;
}

} // namespace NovelTea
