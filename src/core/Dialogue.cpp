#include <NovelTea/Dialogue.hpp>
#include <iostream>

namespace NovelTea
{

Dialogue::Dialogue()
: m_name("New Dialogue")
{
}

json Dialogue::toJson() const
{
	auto j = json::array({
		m_name,
	});
	return j;
}

bool Dialogue::fromJson(const json &j)
{
	if (!j.is_array() || j.size() != 1)
		return false;

	try
	{
		m_name = j[0];
		return true;
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return false;
	}
}

} // namespace NovelTea
