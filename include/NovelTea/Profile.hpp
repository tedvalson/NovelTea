#ifndef NOVELTEA_PROFILE_HPP
#define NOVELTEA_PROFILE_HPP

#include <NovelTea/Utils.hpp>

namespace NovelTea
{

class Profile
{
public:
	Profile(int index);

	ADD_ACCESSOR(std::string, Name, m_name)

private:
	int m_index;
	std::string m_name;
};

} // namespace NovelTea

#endif // NOVELTEA_PROFILE_HPP
