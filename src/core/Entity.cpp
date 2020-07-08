#include <NovelTea/Entity.hpp>
#include <iostream>

namespace NovelTea
{

bool Entity::fromJson(const json &j)
{
	if (!j.is_array() || j.size() != jsonSize())
		return false;

	try
	{
		loadJson(j);
		return true;
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return false;
	}
}

} // namespace NovelTea
