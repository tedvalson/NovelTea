#include <NovelTea/Err.hpp>
#include <iostream>

namespace NovelTea
{

std::ostream &err()
{
	return std::cerr;
}

std::ostream &warn()
{
	return std::cerr;
}

} // namespace NovelTea
