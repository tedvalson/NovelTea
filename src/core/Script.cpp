#include <NovelTea/Script.hpp>
#include <iostream>

namespace NovelTea
{

Script::Script()
{
}

json Script::toJson() const
{
	auto j = json::array({
		m_id,
		m_content,
	});
	return j;
}

bool Script::fromJson(const json &j)
{
	if (!j.is_array() || j.size() != 2)
		return false;

	try
	{
		m_id = j[0];
		m_content = j[1];
		return true;
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return false;
	}
}

} // namespace NovelTea
