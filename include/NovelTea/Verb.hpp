#ifndef NOVELTEA_VERB_HPP
#define NOVELTEA_VERB_HPP

#include <NovelTea/JsonSerializable.hpp>
#include <NovelTea/Utils.hpp>
#include <vector>

namespace NovelTea
{

class Verb : public JsonSerializable
{
public:
	Verb();
	json toJson() const override;
	bool fromJson(const json &j) override;

	ADD_ACCESSOR(std::string, Name, m_name)

private:
	std::string m_name;
};

} // namespace NovelTea

#endif // NOVELTEA_VERB_HPP
