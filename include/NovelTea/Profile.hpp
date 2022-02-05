#ifndef NOVELTEA_PROFILE_HPP
#define NOVELTEA_PROFILE_HPP

#include <NovelTea/Utils.hpp>

namespace NovelTea
{

class Profile
{
public:
	Profile(const std::string &name);

	ADD_ACCESSOR(std::string, Name, m_name)

private:
	std::string m_name;
};

} // namespace NovelTea

#endif // NOVELTEA_PROFILE_HPP
