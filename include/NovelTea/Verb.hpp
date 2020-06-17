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
	ADD_ACCESSOR(int, ObjectCount, m_objectCount)
	ADD_ACCESSOR(std::vector<std::string>, ActionStructure, m_actionStructure)
	ADD_ACCESSOR(std::string, DefaultScriptSuccess, m_defaultScriptSuccess)
	ADD_ACCESSOR(std::string, DefaultScriptFailure, m_defaultScriptFailure)
	ADD_ACCESSOR(json, Properties, m_properties)

private:
	std::string m_name;
	int m_objectCount;
	std::vector<std::string> m_actionStructure;
	std::string m_defaultScriptSuccess;
	std::string m_defaultScriptFailure;
	json m_properties;
};

} // namespace NovelTea

#endif // NOVELTEA_VERB_HPP
